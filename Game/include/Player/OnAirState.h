#pragma once

#include "Player/PlayerState.h"

namespace Game
{
    class OnAirState : public PlayerState
    {
    public:
        void Exit(PlayerController& playerController) override;
    protected:
        void OnUpdate(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState) override;
        bool IsGround(PlayerController& playerController, const PlayerInputState& inputState);
        bool CanAttack(PlayerController& playerController, const PlayerInputState& inputState);
    };
}