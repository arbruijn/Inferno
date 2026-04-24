#include "pch.h"
#include "Game.PsxMovie.h"

#include <algorithm>
#include <fstream>

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
        constexpr float DEFAULT_MOVIE_FRAME_TIME = 1.0f / 30.0f;

        struct PsxMovieState {
            Psx::PsxIso9660Reader Iso;
            std::unique_ptr<Psx::PsxReadStream> Stream;
            Psx::PsxStrReader Reader;
            Psx::PsxPlaybackSession Playback;
            Psx::PsxRgbFrame CurrentFrame;
            std::vector<uint32> RgbaPixels;
            Texture2D FrameTexture;
            float FrameTime = DEFAULT_MOVIE_FRAME_TIME;
            float Accumulator = 0.0f;
            bool HasFrame = false;
            bool FrameDirty = false;

            void Reset() {
                Iso.Clear();
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
            }

            bool LoadFrame(std::string* error) {
                Psx::PsxPlaybackBufferedFrame buffered;
                if (!Playback.HasBufferedFrames()) {
                    if (!Playback.FillVideoBuffer(8, error)) {
                        return false;
                    }
                }

                if (!Playback.HasBufferedFrames()) {
                    return false;
                }

                if (!Playback.TakeFrontVideoFrame(&buffered, error)) {
                    return false;
                }

                CurrentFrame = std::move(buffered.frame);
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
                std::ifstream image(isoPath, std::ios::binary);
                if (!image) {
                    if (error) *error = "Could not open PSX image: " + isoPath.string();
                    return false;
                }

                if (!Iso.Load(image, error)) {
                    return false;
                }

                Stream = Iso.OpenFile(filename, Psx::kPsxMode2_2336, error);
                if (!Stream) {
                    return false;
                }

                if (!Reader.Load(Stream.get(), error)) {
                    return false;
                }

                if (!Playback.Start(&Reader, error)) {
                    return false;
                }

                const float cadence = static_cast<float>(Playback.Cadence().frameDurationSeconds);
                if (cadence > 0.0f) {
                    FrameTime = cadence;
                }

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
