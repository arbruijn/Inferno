#include "pch.h"
#include "Game.PsxMovie.h"

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <fstream>
#include <deque>
#include <memory>
#include <mutex>

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
        constexpr float DEFAULT_MOVIE_FRAME_TIME = 1.0f / 15.0f;
        constexpr std::size_t VIDEO_BUFFER_TARGET = 2;
        constexpr std::size_t AUDIO_BUFFER_TARGET = 1;

        struct PsxMovieState {
            using AudioBuffer = std::shared_ptr<std::vector<std::int16_t>>;

            Psx::PsxIso9660Reader Iso;
            std::unique_ptr<std::ifstream> Image;
            std::unique_ptr<Psx::PsxReadStream> Stream;
            Psx::PsxStrReader Reader;
            Psx::PsxPlaybackSession Playback;
            Psx::PsxRgbFrame CurrentFrame;
            std::vector<uint32> RgbaPixels;
            Texture2D FrameTexture;
            std::unique_ptr<DynamicSoundEffectInstance> Audio;
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

            void StopAudio() {
                std::scoped_lock lock(AudioMutex);
                if (Audio) {
                    Audio->Stop();
                }
                Audio.reset();
                PendingAudioPackets.clear();
                LiveAudioBuffers.clear();
                AudioStarted = false;
                AudioSampleRate = 0;
                AudioChannelCount = 0;
                LoggedAudioBufferSubmissions = 0;
                AudioEnabled = true;
            }

            void ReleaseCompletedAudioBuffersLocked() {
                if (!Audio) {
                    return;
                }

                AudioRunning = true;

                const auto pendingBufferCount = static_cast<std::size_t>(std::max(Audio->GetPendingBufferCount(), 0));
                while (LiveAudioBuffers.size() > pendingBufferCount) {
                    LiveAudioBuffers.pop_front();
                }
            }

            void RefillAudioBuffersLocked() {
                if (!Audio || !AudioEnabled) {
                    return;
                }

                // DynamicSoundEffectInstance reuses the same callback for both
                // real OnBufferEnd notifications and the initial "buffer needed"
                // wake-up from Play(). Retire submitted PCM by comparing it with
                // XAudio2's actual queued buffer count instead of assuming one
                // buffer completed per callback.
                ReleaseCompletedAudioBuffersLocked();

                int pendingBufferCount = Audio->GetPendingBufferCount();

                while (pendingBufferCount < static_cast<int>(AUDIO_BUFFER_TARGET) && !PendingAudioPackets.empty()) {
                    auto pending = std::move(PendingAudioPackets.front());
                    PendingAudioPackets.pop_front();

                    auto buffer = std::make_shared<std::vector<std::int16_t>>(std::move(pending.pcm.samples));
                    if (buffer->empty()) {
                        continue;
                    }

                    if (LoggedAudioBufferSubmissions < 8) {
                        const double bufferDurationMs =
                            pending.pcm.sampleRate > 0
                                ? 1000.0 * static_cast<double>(pending.pcm.sampleFrames) / static_cast<double>(pending.pcm.sampleRate)
                                : 0.0;
                        SPDLOG_INFO("Queued PSX movie audio buffer #{}: {} sample frames, {} channels, {:.3f} ms, XA queue depth {} -> {}",
                                    LoggedAudioBufferSubmissions + 1,
                                    pending.pcm.sampleFrames,
                                    pending.pcm.channelCount,
                                    bufferDurationMs,
                                    pendingBufferCount,
                                    pendingBufferCount + 1);
                        ++LoggedAudioBufferSubmissions;
                    }

                    const auto byteCount = buffer->size() * sizeof((*buffer)[0]);
                    Audio->SubmitBuffer(reinterpret_cast<const std::uint8_t*>(buffer->data()), byteCount);
                    LiveAudioBuffers.push_back(std::move(buffer));
                    ++pendingBufferCount;
                }

                if (!AudioStarted && pendingBufferCount > 0) {
                    Audio->Play();
                    AudioStarted = true;
                }
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

                    if (!Audio) {
                        auto* engine = Sound::GetEngine();
                        if (engine == nullptr) {
                            AudioEnabled = false;
                            return;
                        }

                        AudioSampleRate = packet.pcm.sampleRate;
                        AudioChannelCount = packet.pcm.channelCount;
                        try {
                            Audio = std::make_unique<DynamicSoundEffectInstance>(
                                engine,
                                [this](DynamicSoundEffectInstance*) {
                                    std::scoped_lock callbackLock(AudioMutex);
                                    RefillAudioBuffersLocked();
                                },
                                AudioSampleRate,
                                AudioChannelCount,
                                16);
                            Audio->SetVolume(1.0f);
                        } catch (const std::exception& ex) {
                            SPDLOG_WARN("Unable to start PSX movie audio playback: {}", ex.what());
                            AudioEnabled = false;
                            Audio.reset();
                            PendingAudioPackets.clear();
                            LiveAudioBuffers.clear();
                            return;
                        }
                    }

                    if (packet.pcm.sampleRate != AudioSampleRate || packet.pcm.channelCount != AudioChannelCount) {
                        SPDLOG_WARN("PSX movie audio format changed during playback; disabling audio.");
                        AudioEnabled = false;
                        PendingAudioPackets.clear();
                        LiveAudioBuffers.clear();
                        if (Audio) {
                            Audio->Stop();
                            Audio.reset();
                        }
                        return;
                    }

                    PendingAudioPackets.push_back(std::move(packet));
                }

                if (Audio) {
                    RefillAudioBuffersLocked();
                }
            }

            bool LoadFrame(std::string* error) {
                Psx::PsxPlaybackBufferedFrame buffered;
                if (Playback.BufferedFrameCount() < VIDEO_BUFFER_TARGET) {
                    if (!Playback.FillVideoBuffer(VIDEO_BUFFER_TARGET, error)) {
                        DrainQueuedAudioPackets();
                        return false;
                    }
                    DrainQueuedAudioPackets();
                }

                if (!Playback.HasBufferedFrames()) {
                    DrainQueuedAudioPackets();
                    return false;
                }

                if (!AudioRunning) {
                    if (!CurrentFrame.width) {
                        auto frame = Playback.PeekNextFrame();
                        CurrentFrame.width = frame->frame.width;
                        CurrentFrame.height = frame->frame.height;
                        const std::size_t pixelCount = static_cast<std::size_t>(CurrentFrame.width) * CurrentFrame.height;
                        RgbaPixels.resize(pixelCount);
                        memset(&RgbaPixels[0], 0, pixelCount * sizeof(RgbaPixels[0]));
                    }
                    return true;
                }

                if (!Playback.TakeFrontVideoFrame(&buffered, error)) {
                    DrainQueuedAudioPackets();
                    return false;
                }

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

                DrainQueuedAudioPackets();

                if (!LoadFrame(error)) {
                    return false;
                }

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
        auto& movie = Movie();
        if (!PsxMovieVisible) {
            return;
        }

        movie.DrainQueuedAudioPackets();

        movie.Accumulator += dt;
        while (movie.Accumulator >= movie.FrameTime) {
            movie.Accumulator -= movie.FrameTime;

            std::string error;
            if (!movie.LoadFrame(&error)) {
                if (!error.empty()) {
                    SPDLOG_WARN("PSX movie playback ended or failed: {}", error);
                    movie.Stop();
                }
                Game::SetState(GameState::LoadLevel);
                return;
            }
        }
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
