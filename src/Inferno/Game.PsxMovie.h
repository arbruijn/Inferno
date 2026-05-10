#pragma once

#include <functional>
#include <string>

namespace Inferno {
    class GraphicsContext;
    class RenderTarget;

    // True while a PSX STR movie is active and should be rendered instead of the briefing UI.
    inline bool PsxMovieVisible = false;

    bool ShowPsxMovie(const std::string& filename, std::function<void()> onEnded = {});
    void StopPsxMovie();
    void UpdatePsxMovie(float dt);
    void HandlePsxMovieInput();
    void DrawPsxMovie(GraphicsContext& ctx, RenderTarget& target);
}
