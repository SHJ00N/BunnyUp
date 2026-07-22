#pragma once

#include "Player/PlayerState.h"

namespace Game
{
    class DamageState : public PlayerState
    {
    public:
        bool HandleInput(PlayerController& playerController, const PlayerInputState& inputState) override;
        void Enter(PlayerController& playerController) override;
        void Exit(PlayerController& playerController) override;
    };
}
