#pragma once

#include "Player/PlayerState.h"

namespace Game
{
    class OnAirState : public PlayerState
    {
        public:
        bool HandleInput(PlayerController& playerController, const PlayerInputState& inputState) override;
        void Update(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState) override;
        void Exit(PlayerController& playerController) override;
    };
}