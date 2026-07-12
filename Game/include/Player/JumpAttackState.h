#pragma once

#include "Player/OnAirState.h"

namespace Game
{
    class JumpAttackState : public OnAirState
    {
    public:
        bool HandleInput(PlayerController& playerController, const PlayerInputState& inputState) override;
        void Enter(PlayerController& playerController) override;
        void Exit(PlayerController& playerController) override;
    protected:
        void OnUpdate(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState) override;
    };
}