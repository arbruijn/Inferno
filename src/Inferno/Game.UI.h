#pragma once
#include "Difficulty.h"
#include "Mission.h"
#include "Game.UI.ScoreScreen.h"
#include <functional>

namespace Inferno::UI {
    using MissionSelectContinuation = std::function<void(MissionInfo&, int, DifficultyLevel)>;

    void Update();
    void ShowPauseDialog();
    void ShowScoreScreen(const ScoreInfo& score, bool secretLevel);
    void ShowMainMenu();
    void ShowMultiplayerDialog();
    void ShowStartGameFlow(MissionSelectContinuation continuation);

    // missionFailed indicates the player ran out of lives
    void ShowFailedEscapeDialog(bool missionFailed);
}
