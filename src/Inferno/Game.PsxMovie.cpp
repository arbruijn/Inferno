#include "pch.h"
#include "Game.PsxMovie.h"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <deque>
#include <memory>
#include <mutex>
#include <vector>

#include "Game.h"
#include "Game.Bindings.h"
#include "Graphics/Render.h"
#include "Graphics/Render.Canvas.h"
#include "Graphics/Render.Briefing.h"
#include "Psx/PsxIso9660.h"
#include "Psx/PsxPlayback.h"
#include "Psx/PsxXaAudio.h"
#include "SoundSystem.h"

namespace Inferno {
    namespace {
        constexpr float DEFAULT_MOVIE_FRAME_TIME = 1.0f / 15.0f * 1000.0f / 1001.0f;
        constexpr std::size_t VIDEO_BUFFER_TARGET = 3;
        constexpr std::size_t AUDIO_BUFFER_TARGET = 3;
        constexpr float MIN_ADAPTIVE_FRAME_TIME_SCALE = 1.0f;
        constexpr float MAX_ADAPTIVE_FRAME_TIME_SCALE = 1.5f;
        constexpr int AUDIO_QUEUE_CLEAR_FRAMES_BEFORE_PROBE = 8;

        struct PsxMovieState {
            struct AudioBuffer {
                std::shared_ptr<std::vector<std::int16_t>> Samples;
                std::uint64_t SampleFrames = 0;
            };

            Psx::PsxIso9660Reader Iso;
            std::unique_ptr<std::ifstream> Image;
            std::unique_ptr<Psx::PsxReadStream> Stream;
            Psx::PsxStrReader Reader;
            Psx::PsxPlaybackSession Playback;
            Psx::PsxRgbFrame CurrentFrame;
            std::vector<uint32> RgbaPixels;
            Texture2D FrameTexture;
            std::unique_ptr<DynamicSoundEffectInstance> AudioVoice;
            std::deque<Psx::PsxPlaybackAudioPacket> PendingAudioPackets;
            std::deque<AudioBuffer> LiveAudioBuffers;
            std::mutex AudioMutex;
            float FrameTime = DEFAULT_MOVIE_FRAME_TIME;
            float TargetFrameTime = DEFAULT_MOVIE_FRAME_TIME;
            float AdaptiveFrameTime = DEFAULT_MOVIE_FRAME_TIME;
            float Accumulator = 0.0f;
            bool HasFrame = false;
            bool FrameDirty = false;
            std::atomic<bool> AudioEnabled = true;
            bool AudioStarted = false;
            bool AudioRunning = false;
            int AudioSampleRate = 0;
            std::uint8_t AudioChannelCount = 0;
            std::size_t LoggedAudioBufferSubmissions = 0;
            std::uint64_t CompletedAudioSampleFrames = 0;
            std::uint64_t QueuedAudioSampleFrames = 0;
            int ConsecutiveAudioQueueClearFrames = 0;
            bool Presenting = false;
            int lastFrameNum_ = 0;
            int submit_count = 0;

            bool SetupEmptyFrame() {
                auto frame = Playback.PeekNextFrame();
                if (!frame)
                    return false;
                CurrentFrame.width = frame->frame.width;
                CurrentFrame.height = frame->frame.height;
                const std::size_t pixelCount = static_cast<std::size_t>(CurrentFrame.width) * CurrentFrame.height;
                RgbaPixels.resize(pixelCount);
                memset(&RgbaPixels[0], 0, pixelCount * sizeof(RgbaPixels[0]));
                HasFrame = true;
                FrameDirty = true;
                return true;
            }

            void Reset() {
                StopAudio();
                Iso.Clear();
                Image.reset();
                Stream.reset();
                Reader.Clear();
                Playback = {};
                CurrentFrame = {};
                RgbaPixels.clear();
                FrameTexture.Release();
                FrameTime = DEFAULT_MOVIE_FRAME_TIME;
                TargetFrameTime = DEFAULT_MOVIE_FRAME_TIME;
                AdaptiveFrameTime = DEFAULT_MOVIE_FRAME_TIME;
                Accumulator = 0.0f;
                HasFrame = false;
                FrameDirty = false;
                AudioEnabled = true;
            }

            void ClearQueuedAudioState() {
                std::scoped_lock lock(AudioMutex);
                ClearQueuedAudioStateLocked();
            }

            void ClearQueuedAudioStateLocked() {
                PendingAudioPackets.clear();
                LiveAudioBuffers.clear();
                QueuedAudioSampleFrames = 0;
                ConsecutiveAudioQueueClearFrames = 0;
            }

            void SetNominalFrameTime(float frameTime) {
                if (frameTime <= 0.0f) {
                    return;
                }

                const float oldFrameTime = FrameTime > 0.0f ? FrameTime : frameTime;
                const float ratio = frameTime / oldFrameTime;
                FrameTime = frameTime;
                TargetFrameTime = std::clamp(TargetFrameTime * ratio,
                                             FrameTime * MIN_ADAPTIVE_FRAME_TIME_SCALE,
                                             FrameTime * MAX_ADAPTIVE_FRAME_TIME_SCALE);
                AdaptiveFrameTime = std::clamp(AdaptiveFrameTime * ratio,
                                               FrameTime * MIN_ADAPTIVE_FRAME_TIME_SCALE,
                                               FrameTime * MAX_ADAPTIVE_FRAME_TIME_SCALE);
            }

            void DestroyAudioVoice(std::unique_ptr<DynamicSoundEffectInstance>& voice) {
                if (!voice) {
                    return;
                }

                voice->Stop(true);
                voice.reset();
            }

            void StopAudio() {
                std::unique_ptr<DynamicSoundEffectInstance> voiceToDestroy;
                {
                    std::scoped_lock lock(AudioMutex);
                    voiceToDestroy = std::move(AudioVoice);
                    AudioStarted = false;
                    AudioRunning = false;
                    AudioSampleRate = 0;
                    AudioChannelCount = 0;
                    LoggedAudioBufferSubmissions = 0;
                    submit_count = 0;
                    AudioEnabled = true;
                }

                if (voiceToDestroy) {
                    DestroyAudioVoice(voiceToDestroy);
                }

                ClearQueuedAudioState();
            }

            void ReleaseCompletedAudioBuffers() {
                std::scoped_lock lock(AudioMutex);
                ReleaseCompletedAudioBuffersLocked();
            }

            void ReleaseCompletedAudioBuffersLocked() {
                if (!AudioVoice) {
                    return;
                }
                AudioRunning = true;

                const auto pendingBufferCount = static_cast<std::size_t>(std::max(GetAudioPendingBufferCount(), 0));
                while (LiveAudioBuffers.size() > pendingBufferCount) {
                    LiveAudioBuffers.pop_front();
                }
            }

            bool RefillAudioBuffers() {
                std::scoped_lock lock(AudioMutex);
                return RefillAudioBuffersLocked();
            }

            bool RefillAudioBuffersLocked() {
                if (!AudioVoice || !AudioEnabled) {
                    return true;
                }

                ReleaseCompletedAudioBuffersLocked();

                int pendingBufferCount = GetAudioPendingBufferCount();

                while (pendingBufferCount < static_cast<int>(AUDIO_BUFFER_TARGET) && !PendingAudioPackets.empty()) {
                    auto pending = std::move(PendingAudioPackets.front());
                    PendingAudioPackets.pop_front();

                    auto samples = std::make_shared<std::vector<std::int16_t>>(std::move(pending.pcm.samples));
                    if (samples->empty()) {
                        continue;
                    }

                    if (LoggedAudioBufferSubmissions < 8) {
                        const double bufferDurationMs = 1000.0 * pending.durationSeconds;
                        SPDLOG_INFO("Queued PSX movie audio buffer #{}: {} sample frames @ {} Hz, {} channels, {:.3f} ms, XA queue depth {} -> {}",
                                    LoggedAudioBufferSubmissions + 1,
                                    pending.pcm.sampleFrames,
                                    pending.pcm.sampleRate,
                                    pending.pcm.channelCount,
                                    bufferDurationMs,
                                    pendingBufferCount,
                                    pendingBufferCount + 1);
                        ++LoggedAudioBufferSubmissions;
                    }

                    const auto byteCount = samples->size() * sizeof((*samples)[0]);
                    try {
                        AudioVoice->SubmitBuffer(reinterpret_cast<const uint8_t*>(samples->data()), byteCount);
                    } catch (const std::exception& ex) {
                        SPDLOG_WARN("Unable to submit PSX movie audio buffer: {}", ex.what());
                        AudioEnabled = false;
                        AudioVoice.reset();
                        AudioStarted = false;
                        AudioRunning = false;
                        return false;
                    }
                    AudioBuffer liveBuffer;
                    liveBuffer.Samples = std::move(samples);
                    liveBuffer.SampleFrames = pending.pcm.sampleFrames;
                    LiveAudioBuffers.push_back(std::move(liveBuffer));
                    ++pendingBufferCount;
                }

                if (!AudioStarted && pendingBufferCount > 0) {
                    try {
                        AudioVoice->Play();
                    } catch (const std::exception& ex) {
                        SPDLOG_WARN("Unable to start PSX movie audio playback: {}", ex.what());
                        AudioEnabled = false;
                        AudioVoice.reset();
                        AudioStarted = false;
                        AudioRunning = false;
                        return false;
                    }
                    AudioStarted = true;
                }

                return true;
            }

            int GetAudioPendingBufferCount() const {
                if (!AudioVoice) {
                    return 0;
                }

                return AudioVoice->GetPendingBufferCount();
            }

            std::uint64_t GetQueuedAudioSampleFramesLocked() const {
                std::uint64_t queuedSampleFrames = 0;
                int skip = 1;
                for (const auto& packet : PendingAudioPackets) {
                    if (skip) {
                        skip--;
                        continue;
                    }
                    queuedSampleFrames += packet.pcm.sampleFrames;
                }
                return queuedSampleFrames;
            }

            std::uint64_t GetQueuedAudioSampleFrames() {
                std::scoped_lock lock(AudioMutex);
                QueuedAudioSampleFrames = GetQueuedAudioSampleFramesLocked();
                return QueuedAudioSampleFrames;
            }

            float GetAdaptiveFrameTime(float dt) {
                if (!AudioRunning || FrameTime <= 0.0f) {
                    TargetFrameTime = FrameTime;
                    AdaptiveFrameTime = FrameTime;
                    ConsecutiveAudioQueueClearFrames = 0;
                    return FrameTime;
                }

                const auto queuedSampleFrames = GetQueuedAudioSampleFrames();
                const float minFrameTime = FrameTime * MIN_ADAPTIVE_FRAME_TIME_SCALE;
                const float maxFrameTime = FrameTime * MAX_ADAPTIVE_FRAME_TIME_SCALE;

                if (queuedSampleFrames > 0) {
                    ConsecutiveAudioQueueClearFrames = 0;

                    const double queuedSeconds = AudioSampleRate > 0
                        ? static_cast<double>(queuedSampleFrames) / static_cast<double>(AudioSampleRate)
                        : static_cast<double>(FrameTime);
                    const float pressure = std::clamp(static_cast<float>(queuedSeconds / FrameTime), 1.0f, 4.0f);
                    TargetFrameTime += FrameTime * 0.01f * pressure;
                } else {
                    ++ConsecutiveAudioQueueClearFrames;

                    if (ConsecutiveAudioQueueClearFrames >= AUDIO_QUEUE_CLEAR_FRAMES_BEFORE_PROBE) {
                        TargetFrameTime -= FrameTime * 0.001f;
                    }
                }

                TargetFrameTime = std::clamp(TargetFrameTime, minFrameTime, maxFrameTime);

                const float adjustment = std::clamp(dt * 2.5f, 0.02f, 0.12f);
                AdaptiveFrameTime += (TargetFrameTime - AdaptiveFrameTime) * adjustment;
                if (std::abs(TargetFrameTime - AdaptiveFrameTime) < 0.00001f) {
                    AdaptiveFrameTime = TargetFrameTime;
                }

                return AdaptiveFrameTime;
            }

            bool CreateAudioVoice() {
                auto* engine = Sound::GetEngine();
                if (engine == nullptr || engine->GetInterface() == nullptr) {
                    return false;
                }

                WAVEFORMATEX format{};
                format.wFormatTag = WAVE_FORMAT_PCM;
                format.nChannels = AudioChannelCount;
                format.nSamplesPerSec = AudioSampleRate;
                format.wBitsPerSample = 16;
                format.nBlockAlign = static_cast<WORD>(format.nChannels * (format.wBitsPerSample / 8));
                format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

                try {
                    AudioVoice = std::make_unique<DynamicSoundEffectInstance>(engine, nullptr, format.nSamplesPerSec, format.nChannels, format.wBitsPerSample);
                } catch (const std::exception& ex) {
                    SPDLOG_WARN("Unable to start PSX movie audio playback: AllocateVoice failed: {}", ex.what());
                    return false;
                }

                if (!AudioVoice) {
                    SPDLOG_WARN("Unable to start PSX movie audio playback: DynamicSoundEffectInstance returned no voice");
                    return false;
                }
                return true;
            }

            void DrainQueuedAudioPackets() {
                auto packets = Playback.TakeQueuedAudioPackets();
                if (packets.empty()) {
                    RefillAudioBuffers();
                    return;
                }

                if (!AudioEnabled) {
                    return;
                }

                if (!packets.empty() && !AudioVoice) {
                    AudioSampleRate = packets[0].pcm.sampleRate;
                    AudioChannelCount = packets[0].pcm.channelCount;
                    if (!CreateAudioVoice()) {
                        AudioEnabled = false;
                        ClearQueuedAudioState();
                        return;
                    }
                }

                std::unique_ptr<DynamicSoundEffectInstance> voiceToDestroy;
                {
                    std::scoped_lock lock(AudioMutex);
                    for (auto& packet : packets) {
                        if (packet.pcm.samples.empty() || packet.pcm.sampleRate <= 0 || packet.pcm.channelCount == 0) {
                            continue;
                        }

                        if (packet.pcm.sampleRate != AudioSampleRate || packet.pcm.channelCount != AudioChannelCount) {
                            SPDLOG_WARN("PSX movie audio format changed during playback; disabling audio.");
                            AudioEnabled = false;
                            voiceToDestroy = std::move(AudioVoice);
                            break;
                        }

                        PendingAudioPackets.push_back(std::move(packet));
                    }

                    if (!voiceToDestroy && AudioVoice) {
                        if (!RefillAudioBuffersLocked()) {
                            voiceToDestroy = std::move(AudioVoice);
                        }
                    }
                }

                if (voiceToDestroy) {
                    DestroyAudioVoice(voiceToDestroy);
                    ClearQueuedAudioState();
                }
            }

            bool LoadFrame(std::string* error) {
                static int num_frames = 0;
                Psx::PsxPlaybackBufferedFrame buffered;
                if (Playback.BufferedFrameCount() < VIDEO_BUFFER_TARGET) {
                    if (!Playback.FillVideoBuffer(VIDEO_BUFFER_TARGET, error)) {
                        return false;
                    }
                    DrainQueuedAudioPackets();
                }

                if (!Playback.HasBufferedFrames()) {
                    return false;
                }

                if (!AudioRunning) {
                    if (!HasFrame) {
                        SetupEmptyFrame();
                    }
                    return true;
                }

                if (!Playback.TakeFrontVideoFrame(&buffered, error)) {
                    return false;
                }

                lastFrameNum_ = buffered.frameNumber;

                DrainQueuedAudioPackets();

                CurrentFrame = std::move(buffered.frame);
                if (buffered.durationSeconds > 0.0) {
                    SetNominalFrameTime(static_cast<float>(buffered.durationSeconds));
                }
                if (CurrentFrame.width == 0 || CurrentFrame.height == 0 || CurrentFrame.pixels.empty()) {
                    if (error) *error = "Encountered an empty PSX movie frame.";
                    return false;
                }
                HasFrame = true;
                FrameDirty = true;

                const std::size_t pixelCount = static_cast<std::size_t>(CurrentFrame.width) * CurrentFrame.height;
                RgbaPixels.resize(pixelCount);
                const auto* src = CurrentFrame.pixels.data();
                for (std::size_t i = 0; i < pixelCount; ++i) {
                    const std::size_t srcIndex = i * 3;
                    RgbaPixels[i] =
                        static_cast<uint32>(src[srcIndex]) |
                        (static_cast<uint32>(src[srcIndex + 1]) << 8U) |
                        (static_cast<uint32>(src[srcIndex + 2]) << 16U) |
                        0xFF000000U;
                }

                return true;
            }

            bool EnsureTexture() {
                if (!HasFrame) {
                    return false;
                }

                if (!FrameTexture || FrameTexture.GetWidth() != CurrentFrame.width || FrameTexture.GetHeight() != CurrentFrame.height) {
                    FrameTexture.Create(CurrentFrame.width, CurrentFrame.height, "PSX movie frame");
                    FrameTexture.AddShaderResourceView();
                    FrameDirty = true;
                }

                return true;
            }

            bool OpenMovieStream(const std::string& filename, std::string* error) {
                const auto isoPath = D1_FOLDER / "psx.bin";
                Image = std::make_unique<std::ifstream>(isoPath, std::ios::binary);
                if (!*Image) {
                    if (error) *error = "Could not open PSX image: " + isoPath.string();
                    Image.reset();
                    return false;
                }

                if (!Iso.Load(*Image, error)) {
                    Image.reset();
                    return false;
                }

                Stream = Iso.OpenFile(filename, Psx::kPsxMode2_2352, error);
                if (!Stream) {
                    return false;
                }

                if (!Reader.Load(Stream.get(), error)) {
                    return false;
                }

                if (!Playback.Start(&Reader, error)) {
                    return false;
                }

                if (!Playback.FillVideoBuffer(VIDEO_BUFFER_TARGET, error)) {
                     return false;
                }

                SetupEmptyFrame();

                return true;
            }

            void Stop() {
                Reset();
                PsxMovieVisible = false;
            }
        };

        PsxMovieState& Movie() {
            static PsxMovieState movie;
            return movie;
        }

        bool IsDismissPressed() {
            return Input::OnKeyPressed(Input::Keys::Escape);
        }

        void UploadFrame(GraphicsContext& ctx) {
            auto& movie = Movie();
            if (!movie.HasFrame || !movie.FrameDirty) {
                return;
            }

            if (!movie.EnsureTexture()) {
                return;
            }

            auto cmdList = ctx.GetCommandList();
            movie.FrameTexture.CopyFrom(cmdList, movie.RgbaPixels.data());
            movie.FrameTexture.Transition(cmdList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            movie.FrameDirty = false;
        }
    } // namespace

    bool ShowPsxMovie(const std::string& filename) {
        auto& movie = Movie();
        movie.Stop();

        std::string error;
        if (!movie.OpenMovieStream(filename, &error)) {
            if (!error.empty()) {
                SPDLOG_WARN("Unable to load PSX movie {}: {}", filename, error);
            }
            movie.Stop();
            return false;
        }

        Game::BriefingVisible = false;
        PsxMovieVisible = true;
        Game::SetState(GameState::Briefing);
        Psx::StopXaAudio();
        Sound::StopMusic();
        return true;
    }

    void StopPsxMovie() {
        Movie().Stop();
    }

    void PsxMovieSetNextState() {
        //Game::SetState(GameState::LoadLevel);
        auto mission = Game::GetCurrentMissionInfo();
        if (mission) {
            auto briefingName = mission->GetValue("briefing");
            ShowBriefing(*mission, Game::LevelNumber, Game::Level, briefingName, false, true);
        }
    }

    void UpdatePsxMovie(float dt) {
        static int updates = 0;
        static int frames = 0;
        static float totalTime = 0;
        auto& movie = Movie();
        if (!PsxMovieVisible) {
            return;
        }

        movie.DrainQueuedAudioPackets();

        // wait until frametimes are stable
        if (!movie.Presenting && movie.FrameTime > 0.0f && dt > movie.FrameTime) {
            return;
        }

        movie.Presenting = true;

        if (movie.AudioRunning) {
            movie.Accumulator += dt;
            totalTime += dt;
        }
        while (true) {
            const float currentFrameTime = movie.GetAdaptiveFrameTime(dt);
            if (currentFrameTime > 0.0f && movie.Accumulator < currentFrameTime) {
                break;
            }

            if (currentFrameTime > 0.0f) {
                movie.Accumulator -= currentFrameTime;
            }

            std::string error;
            frames++;
            if (!movie.LoadFrame(&error)) {
                if (!error.empty()) {
                    SPDLOG_WARN("PSX movie playback ended or failed: {}", error);
                }
                movie.Stop();
                PsxMovieSetNextState();
                return;
            }
            printf("%.2f frame %d dt %.3f ft %.03f\n", totalTime, movie.lastFrameNum_, dt, currentFrameTime);
            movie.DrainQueuedAudioPackets();
        }
        movie.ReleaseCompletedAudioBuffers();


        updates++;
                char buf[128];
                const auto queuedAudioSamples = movie.GetQueuedAudioSampleFrames();
                snprintf(buf, sizeof(buf), "%.2f m %.2f u %.2f af %.2f tf %.2f bv %zu ba %zu ab %d fn %d qs %llu", (float)frames/updates, 1/movie.FrameTime, 1/dt,
                    1/movie.AdaptiveFrameTime, 1/movie.TargetFrameTime,
                    movie.Playback.BufferedFrameCount(), movie.Playback.BufferedAudioPacketsCount(),
                    movie.GetAudioPendingBufferCount(),
                    movie.lastFrameNum_, static_cast<unsigned long long>(queuedAudioSamples));
                    //movie.LiveAudioBuffers.size()); //movie.Audio->GetPendingBufferCount());
                //SetWindowTextA(GetActiveWindow(), buf);

                snprintf(buf, sizeof(buf), "%.2f m %.2f u %.0f bv %zu ba %zu ab %d vr %.2f ar %.2f fn %d ap %zu", (float)frames/updates, 1/movie.FrameTime, 1/dt,
                    movie.Playback.BufferedFrameCount(), movie.Playback.BufferedAudioPacketsCount(),
                    movie.GetAudioPendingBufferCount(), frames / totalTime, movie.submit_count / totalTime,
                    movie.lastFrameNum_, movie.PendingAudioPackets.size());
                     //movie.LiveAudioBuffers.size()); //movie.Audio->GetPendingBufferCount());
                 SetWindowTextA(GetActiveWindow(), buf);
    }

    void HandlePsxMovieInput() {
        if (!PsxMovieVisible) {
            return;
        }

        if (!IsDismissPressed()) {
            return;
        }

        StopPsxMovie();
        PsxMovieSetNextState();
    }

    void DrawPsxMovie(GraphicsContext& ctx, RenderTarget& target) {
        auto& movie = Movie();
        const Color black(0, 0, 0, 1);
        ctx.ClearColor(target, nullptr, &black);
        if (!PsxMovieVisible || !movie.HasFrame) {
            return;
        }

        UploadFrame(ctx);

        ctx.SetRenderTarget(target.GetRTV());
        ctx.SetViewportAndScissor(target.GetSize());

        const float frameWidth = static_cast<float>(movie.CurrentFrame.width);
        const float frameHeight = static_cast<float>(movie.CurrentFrame.height);
        const auto targetSize = target.GetSize();
        const float targetWidth = static_cast<float>(targetSize.x);
        const float targetHeight = static_cast<float>(targetSize.y);
        const float scale = std::min(targetWidth / frameWidth, targetHeight / frameHeight);

        Render::CanvasBitmapInfo info;
        info.Size = Vector2{ frameWidth * scale, frameHeight * scale };
        info.Position = Vector2{ 0, 0 };
        info.Texture = movie.FrameTexture.GetSRV();
        info.HorizontalAlign = AlignH::Center;
        info.VerticalAlign = AlignV::Center;
        info.Color = Color(1, 1, 1);
        info.PointSample = false;

        Render::Canvas->SetSize(targetSize.x, targetSize.y, targetSize.y);
        Render::Canvas->DrawBitmap(info);
        Render::Canvas->Render(ctx, Render::Heaps->States.LinearClamp());

        target.Transition(ctx.GetCommandList(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}
