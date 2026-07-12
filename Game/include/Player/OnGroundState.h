#pragma once

#include "Player/PlayerState.h"

namespace Game
{
    // OnGroundState is a base class for player states that occur when the player is on the ground (e.g., idle, moving).
    class OnGroundState : public PlayerState
    {
    protected:
        bool CanJump(PlayerController& playerController, const struct PlayerInputState& inputState);
        bool CanFall(PlayerController& playerController);
        bool CanDash(PlayerController& playerController, const struct PlayerInputState& inputState);
        bool CanAttack(PlayerController& playerController, const struct PlayerInputState& inputState);
    };
}