#pragma once

#include "Player/PlayerState.h"

namespace Game
{
    class OnAirState : public PlayerState
    {
    public:
        bool HandleInput(PlayerController& playerController, const PlayerInputState& inputState) override;
        void Exit(PlayerController& playerController) override;
    protected:
        void OnUpdate(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState) override;
        bool IsGround(PlayerController& playerController, const PlayerInputState& inputState);
        bool CanAttack(PlayerController& playerController, const PlayerInputState& inputState);
    };
}