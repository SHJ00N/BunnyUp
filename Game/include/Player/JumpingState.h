#pragma once

#include "Player/OnAirState.h"

namespace Game
{
    class JumpingState : public OnAirState
    {
    public:
        bool HandleInput(PlayerController& playerController, const struct PlayerInputState& inputState) override;
        void Enter(PlayerController& playerController) override;
        void Exit(PlayerController& playerController) override;
    };
}