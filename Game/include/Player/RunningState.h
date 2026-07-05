#pragma once

#include "Player/MovingState.h"

namespace Game
{
    class RunningState : public MovingState
    {
    public:
        bool HandleInput(PlayerController& playerController, const PlayerInputState& inputState) override;
        void Enter(PlayerController& playerController) override;
        void Exit(PlayerController& playerController) override;
    };
}