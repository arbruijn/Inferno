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
#include "SoundSystem.h"


namespace Inferno {
    namespace {
        constexpr float DEFAULT_MOVIE_FRAME_TIME = 1.0f / 15.0f * 1001.0f / 1000.0f;
        constexpr std::size_t VIDEO_BUFFER_TARGET = 2;
        constexpr std::size_t AUDIO_BUFFER_TARGET = 2;
#if 0
        constexpr int PSX_MOVIE_AUDIO_SAMPLE_RATE = 48000;

        bool ResamplePsxMovieAudioPacket(Psx::PsxPlaybackAudioPacket* packet, std::string* error) {
            if (packet == nullptr) {
                if (error != nullptr) {
                    *error = "ResamplePsxMovieAudioPacket requires a non-null packet.";
                }
                return false;
            }

            auto& pcm = packet->pcm;
            if (pcm.samples.empty() || pcm.sampleFrames == 0 || pcm.channelCount == 0 || pcm.sampleRate <= 0) {
                if (error != nullptr) {
                    *error = "Encountered invalid PSX movie audio PCM while resampling.";
                }
                return false;
            }

            if (pcm.sampleRate == PSX_MOVIE_AUDIO_SAMPLE_RATE) {
                packet->durationSeconds =
                    static_cast<double>(pcm.sampleFrames) / static_cast<double>(pcm.sampleRate);
                return true;
            }

            const auto inputFrames = pcm.sampleFrames;
            const auto channelCount = static_cast<std::size_t>(pcm.channelCount);
            const auto inputRate = static_cast<std::uint64_t>(pcm.sampleRate);
            const auto outputRate = static_cast<std::uint64_t>(PSX_MOVIE_AUDIO_SAMPLE_RATE);

            const auto outputFrames = std::max<std::size_t>(
                1,
                static_cast<std::size_t>(
                    (static_cast<std::uint64_t>(inputFrames) * outputRate + inputRate / 2U) / inputRate));

            std::vector<std::int16_t> resampled;
            resampled.resize(outputFrames * channelCount);

            if (inputFrames == 1) {
                for (std::size_t frameIndex = 0; frameIndex < outputFrames; ++frameIndex) {
                    const auto dstBase = frameIndex * channelCount;
                    for (std::size_t channel = 0; channel < channelCount; ++channel) {
                        resampled[dstBase + channel] = pcm.samples[channel];
                    }
                }
            } else {
                const auto sourceSpan = inputFrames - 1;
                const auto targetSpan = outputFrames - 1;

                for (std::size_t frameIndex = 0; frameIndex < outputFrames; ++frameIndex) {
                    const std::uint64_t scaledPosition = static_cast<std::uint64_t>(frameIndex) * sourceSpan;
                    const std::size_t baseFrame = static_cast<std::size_t>(scaledPosition / targetSpan);
                    const std::size_t nextFrame = std::min(baseFrame + 1, sourceSpan);
                    const double fraction = static_cast<double>(scaledPosition % targetSpan) / static_cast<double>(targetSpan);

                    const auto dstBase = frameIndex * channelCount;
                    const auto srcBase0 = baseFrame * channelCount;
                    const auto srcBase1 = nextFrame * channelCount;

                    for (std::size_t channel = 0; channel < channelCount; ++channel) {
                        const double a = static_cast<double>(pcm.samples[srcBase0 + channel]);
                        const double b = static_cast<double>(pcm.samples[srcBase1 + channel]);
                        const auto value = static_cast<int>(std::lround(a + (b - a) * fraction));
                        resampled[dstBase + channel] = static_cast<std::int16_t>(std::clamp(value, -32768, 32767));
                    }
                }
            }

            pcm.samples = std::move(resampled);
            pcm.sampleRate = PSX_MOVIE_AUDIO_SAMPLE_RATE;
            pcm.sampleFrames = outputFrames;
            packet->durationSeconds =
                static_cast<double>(pcm.sampleFrames) / static_cast<double>(pcm.sampleRate);
            return true;
        }
#endif

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
            IXAudio2SourceVoice* AudioVoice = nullptr;
            std::deque<Psx::PsxPlaybackAudioPacket> PendingAudioPackets;
            // Keep submitted PCM alive until XAudio2 signals that each buffer finished.
            std::deque<AudioBuffer> LiveAudioBuffers;
            std::mutex AudioMutex;
            float FrameTime = DEFAULT_MOVIE_FRAME_TIME;
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
            bool Presenting = false;
            int submit_count = 0;
            int lastFrameNum_ = 0;

        uint64_t GetClockTimeNs() const {
            using namespace std::chrono;
            auto time = (uint64_t)duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
            return time;
        }

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
                Accumulator = 0.0f;
                HasFrame = false;
                FrameDirty = false;
                AudioEnabled = true;
            }

            void ClearQueuedAudioStateLocked() {
                PendingAudioPackets.clear();
                LiveAudioBuffers.clear();
                CompletedAudioSampleFrames = 0;
            }

            void StopAudio() {
                std::scoped_lock lock(AudioMutex);
                if (AudioVoice) {
                    std::ignore = AudioVoice->Stop(0);
                    std::ignore = AudioVoice->FlushSourceBuffers();
                    AudioVoice->DestroyVoice();
                    AudioVoice = nullptr;
                }
                ClearQueuedAudioStateLocked();
                AudioStarted = false;
                AudioRunning = false;
                AudioSampleRate = 0;
                AudioChannelCount = 0;
                LoggedAudioBufferSubmissions = 0;
                submit_count = 0;
                AudioEnabled = true;
            }

            void ReleaseCompletedAudioBuffersLocked() {
                if (!AudioVoice) {
                    return;
                }

                #if 0
                static uint64_t last;
                uint64_t cur = GetClockTimeNs();
                if (last) {
                    printf("%f audiobuf\n", (cur - last) / 1e9);
                }
                last = cur;
                #endif

                if (!AudioRunning)
                    printf("audio started\n");
                AudioRunning = true;

                const auto pendingBufferCount = static_cast<std::size_t>(std::max(GetAudioPendingBufferCount(), 0));
                while (LiveAudioBuffers.size() > pendingBufferCount) {
                    CompletedAudioSampleFrames += LiveAudioBuffers.front().SampleFrames;
                    LiveAudioBuffers.pop_front();
                }
            }

            void RefillAudioBuffersLocked() {
                if (!AudioVoice || !AudioEnabled) {
                    return;
                }

                // Retire submitted PCM by comparing it with XAudio2's actual
                // queued buffer count instead of assuming one buffer completed
                // per call to this method.
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
                    XAUDIO2_BUFFER xaBuffer{};
                    xaBuffer.AudioBytes = static_cast<UINT32>(byteCount);
                    xaBuffer.pAudioData = reinterpret_cast<const BYTE*>(samples->data());

                    HRESULT hr = AudioVoice->SubmitSourceBuffer(&xaBuffer, nullptr);
                    if (FAILED(hr)) {
                        SPDLOG_WARN("Unable to submit PSX movie audio buffer to XAudio2.");
                        AudioEnabled = false;
                        ClearQueuedAudioStateLocked();
                        std::ignore = AudioVoice->Stop(0);
                        std::ignore = AudioVoice->FlushSourceBuffers();
                        AudioVoice->DestroyVoice();
                        AudioVoice = nullptr;
                        AudioStarted = false;
                        AudioRunning = false;
                        return;
                    }
                    AudioBuffer liveBuffer;
                    liveBuffer.Samples = std::move(samples);
                    liveBuffer.SampleFrames = pending.pcm.sampleFrames;
                    LiveAudioBuffers.push_back(std::move(liveBuffer));
                    ++pendingBufferCount;
                    submit_count++;
                }

                if (!AudioStarted && pendingBufferCount > 0) {
                    HRESULT hr = AudioVoice->Start(0);
                    if (FAILED(hr)) {
                        SPDLOG_WARN("Unable to start PSX movie audio playback.");
                        AudioEnabled = false;
                        ClearQueuedAudioStateLocked();
                        std::ignore = AudioVoice->Stop(0);
                        std::ignore = AudioVoice->FlushSourceBuffers();
                        AudioVoice->DestroyVoice();
                        AudioVoice = nullptr;
                        AudioStarted = false;
                        AudioRunning = false;
                        return;
                    }
                    AudioStarted = true;
                }
            }

            int GetAudioPendingBufferCount() const {
                if (!AudioVoice) {
                    return 0;
                }

                XAUDIO2_VOICE_STATE state{};
                AudioVoice->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);
                return static_cast<int>(state.BuffersQueued);
            }

            double GetQueuedAudioDepthSecondsLocked() const {
                if (!AudioVoice || AudioSampleRate <= 0) {
                    return 0.0;
                }

                XAUDIO2_VOICE_STATE state{};
                AudioVoice->GetState(&state, 0);

                std::uint64_t queuedSampleFrames = 0;
                for (const auto& buffer : LiveAudioBuffers) {
                    queuedSampleFrames += buffer.SampleFrames;
                }

                const std::uint64_t playedSampleFrames = static_cast<std::uint64_t>(state.SamplesPlayed);
                const std::uint64_t consumedSampleFrames =
                    playedSampleFrames > CompletedAudioSampleFrames
                        ? playedSampleFrames - CompletedAudioSampleFrames
                        : 0;
                const std::uint64_t remainingSampleFrames =
                    queuedSampleFrames > consumedSampleFrames
                        ? queuedSampleFrames - consumedSampleFrames
                        : 0;
                return static_cast<double>(remainingSampleFrames) / static_cast<double>(AudioSampleRate);
            }

            std::size_t GetLiveAudioBufferCount() {
                std::scoped_lock lock(AudioMutex);
                return LiveAudioBuffers.size();
            }

            double GetQueuedAudioDepthSeconds() {
                std::scoped_lock lock(AudioMutex);
                ReleaseCompletedAudioBuffersLocked();
                return GetQueuedAudioDepthSecondsLocked();
            }

            void DrainQueuedAudioPackets() {
                auto packets = Playback.TakeQueuedAudioPackets();
                if (packets.empty()) {
                    return;
                }

                if (!AudioEnabled) {
                    return;
                }

                std::scoped_lock lock(AudioMutex);
                for (auto& packet : packets) {
                    if (packet.pcm.samples.empty() || packet.pcm.sampleRate <= 0 || packet.pcm.channelCount == 0) {
                        continue;
                    }

                    #if 0
                    printf("orig %d hz %d samples\n", packet.pcm.sampleRate, packet.pcm.samples.size());

                    std::string resampleError;
                    if (!ResamplePsxMovieAudioPacket(&packet, &resampleError)) {
                        SPDLOG_WARN("Unable to resample PSX movie audio packet: {}", resampleError);
                        AudioEnabled = false;
                        PendingAudioPackets.clear();
                        LiveAudioBuffers.clear();
                        if (AudioVoice) {
                            std::ignore = AudioVoice->Stop(0);
                            std::ignore = AudioVoice->FlushSourceBuffers();
                            AudioVoice->DestroyVoice();
                            AudioVoice = nullptr;
                        }
                        return;
                    }
                    printf("resampled to %d hz %d samples\n", packet.pcm.sampleRate, packet.pcm.samples.size());
                    #endif

                    if (!AudioVoice) {
                        auto* engine = Sound::GetEngine();
                        if (engine == nullptr || engine->GetInterface() == nullptr) {
                            AudioEnabled = false;
                            return;
                        }

                        AudioSampleRate = packet.pcm.sampleRate;
                        AudioChannelCount = packet.pcm.channelCount;
                        WAVEFORMATEX format{};
                        format.wFormatTag = WAVE_FORMAT_PCM;
                        format.nChannels = AudioChannelCount;
                        format.nSamplesPerSec = AudioSampleRate;
                        format.wBitsPerSample = 16;
                        format.nBlockAlign = static_cast<WORD>(format.nChannels * (format.wBitsPerSample / 8));
                        format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

                        HRESULT hr = engine->GetInterface()->CreateSourceVoice(
                            &AudioVoice,
                            &format,
                            0,
                            XAUDIO2_DEFAULT_FREQ_RATIO,
                            nullptr,
                            nullptr,
                            nullptr);
                        if (FAILED(hr) || !AudioVoice) {
                            SPDLOG_WARN("Unable to start PSX movie audio playback: CreateSourceVoice failed with 0x{:08X}", static_cast<unsigned int>(hr));
                            AudioEnabled = false;
                            ClearQueuedAudioStateLocked();
                            return;
                        }
                    }

                    if (packet.pcm.sampleRate != AudioSampleRate || packet.pcm.channelCount != AudioChannelCount) {
                        SPDLOG_WARN("PSX movie audio format changed during playback; disabling audio.");
                        AudioEnabled = false;
                        ClearQueuedAudioStateLocked();
                        if (AudioVoice) {
                            std::ignore = AudioVoice->Stop(0);
                            std::ignore = AudioVoice->FlushSourceBuffers();
                            AudioVoice->DestroyVoice();
                            AudioVoice = nullptr;
                        }
                        return;
                    }

                    PendingAudioPackets.push_back(std::move(packet));
                }

                if (AudioVoice) {
                    RefillAudioBuffersLocked();
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
                    FrameTime = static_cast<float>(buffered.durationSeconds);
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

                /*
                num_frames++;
                char buf[32];
                snprintf(buf, sizeof(buf), "%d", num_frames);
                SetWindowTextA(GetActiveWindow(), buf);
                */

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
                const auto isoPath = /*"/srv/psxout/INTRO_2352.bin";*/ D1_FOLDER / "psx.bin";
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
            return Input::MouseButtonPressed(Input::MouseButtons::LeftClick) ||
                   Input::MouseButtonPressed(Input::MouseButtons::RightClick) ||
                   Input::OnKeyPressed(Input::Keys::Space) ||
                   Input::OnKeyPressed(Input::Keys::Escape) ||
                   Input::OnKeyPressed(Input::Keys::Left) ||
                   Input::OnKeyPressed(Input::Keys::Right) ||
                   Input::MenuActions.IsSet(MenuAction::Confirm) ||
                   Input::MenuActions.IsSet(MenuAction::Left) ||
                   Input::MenuActions.IsSet(MenuAction::Right) ||
                   Game::Bindings.Pressed(GameAction::Pause);
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
        Sound::StopMusic();
        return true;
    }

    void StopPsxMovie() {
        Movie().Stop();
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

        auto GetEffectiveFrameTime = [&]() {
            const float nominalFrameTime = movie.FrameTime;
            if (!movie.AudioRunning || nominalFrameTime <= 0.0f || movie.Playback.BufferedFrameCount() == 0) {
                return nominalFrameTime;
            }

            const double queuedAudioSeconds = movie.GetQueuedAudioDepthSeconds();
            const double targetQueuedAudioSeconds =
                static_cast<double>(movie.Playback.BufferedFrameCount() - 1) * nominalFrameTime;
            const double correctedFrameTime =
                std::clamp(queuedAudioSeconds - targetQueuedAudioSeconds,
                           0.0,
                           static_cast<double>(nominalFrameTime) * 2.0);
            return static_cast<float>(correctedFrameTime);
        };

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
            const float currentFrameTime = GetEffectiveFrameTime();
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
                Game::SetState(GameState::LoadLevel);
                return;
            }
            printf("%.2f frame %d dt %.3f ft %.03f\n", totalTime, movie.lastFrameNum_, dt, currentFrameTime);
        }


        updates++;
                char buf[80];
                snprintf(buf, sizeof(buf), "%.2f m %.2f u %.2f bv %zu ba %zu ab %d vr %.2f ar %.2f fn %d ap %zu", (float)frames/updates, 1/movie.FrameTime, 1/dt,
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
        Game::SetState(GameState::LoadLevel);
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

        Render::BriefingCanvas->SetSize(640, 480);

        const float frameWidth = static_cast<float>(movie.CurrentFrame.width);
        const float frameHeight = static_cast<float>(movie.CurrentFrame.height);
        const float targetWidth = 640.0f;
        const float targetHeight = 480.0f;
        const float scale = std::min(targetWidth / frameWidth, targetHeight / frameHeight);

        Render::CanvasBitmapInfo info;
        info.Size = Vector2{ frameWidth * scale, frameHeight * scale };
        info.Position = Vector2{ 0, 0 };
        info.Texture = movie.FrameTexture.GetSRV();
        info.HorizontalAlign = AlignH::Center;
        info.VerticalAlign = AlignV::Center;
        info.Color = Color(1, 1, 1);
        Render::BriefingCanvas->DrawBitmap(info);
        Render::BriefingCanvas->Render(ctx, Render::Heaps->States.LinearClamp());

        target.Transition(ctx.GetCommandList(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}
