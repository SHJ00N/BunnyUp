#pragma once

#include "Player/OnGroundState.h"

namespace Game
{
    class CombatState : public OnGroundState
    {
    public:
        bool HandleInput(PlayerController& playerController, const struct PlayerInputState& inputState) override;
        void Enter(PlayerController& playerController) override;
        void Exit(PlayerController& playerController) override;
    };
}