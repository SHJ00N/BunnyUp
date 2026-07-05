#pragma once

#include "Player/PlayerState.h"

namespace Game
{
    // OnGroundState is a base class for player states that occur when the player is on the ground (e.g., idle, moving).
    class OnGroundState : public PlayerState
    {
    public:
        bool HandleInput(PlayerController& playerController, const struct PlayerInputState& inputState) override;
    };
}