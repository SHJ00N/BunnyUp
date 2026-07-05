#pragma once

#include "Player/OnGroundState.h"

namespace Game
{
    class IdleState : public OnGroundState
    {
    public:
        bool HandleInput(PlayerController& playerController, const struct PlayerInputState& inputState) override;
        void Enter(PlayerController& playerController) override;
        void Update(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState) override;
        void Exit(PlayerController& playerController) override;
    };
}