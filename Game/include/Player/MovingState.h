#pragma once

#include "Player/OnGroundState.h"

namespace Game
{
    // MovingState is a base class for player states that involve movement (e.g., walking, running).
    class MovingState : public OnGroundState
    {
    public:
        bool HandleInput(PlayerController& playerController, const PlayerInputState& inputState) override;
        void Update(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState) override;
        void Exit(PlayerController& playerController) override;
    };
}