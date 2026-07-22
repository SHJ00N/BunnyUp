#include "Player/PlayerController.h"
#include "Input/PlayerInputManager.h"
#include "Player/OnGroundState.h"
#include "Common/Health.h"

namespace Game
{
    bool OnGroundState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
    {
        if (playerController.GetHealth()->IsDeath())
        {
            playerController.ChangeState(playerController.GetDeathState());
            return true;
        }

        if (playerController.hasDamage)
        {
            playerController.ChangeState(playerController.GetDamageState());
            return true;
        }

        return false;
    }

    bool OnGroundState::CanJump(PlayerController& playerController, const struct PlayerInputState& inputState)
    {
        // Transition to Jumping state
        if (inputState.jump && playerController.CanJump())
        {
            playerController.ConsumeJump();
            playerController.ChangeState(playerController.GetJumpingState());
            return true;
        }

        return false;
    }

    bool OnGroundState::CanFall(PlayerController& playerController)
    {
        // Transition to Falling state
        if (!playerController.isGrounded && playerController.GetCoyoteTime() <= 0.0f && playerController.GetRigidbody()->GetLinearVelocity().y < -0.1f)
        {
            playerController.ChangeState(playerController.GetFallingState());
            return true;
        }

        return false;
    }

    bool OnGroundState::CanDash(PlayerController& playerController, const struct PlayerInputState& inputState)
    {
        // Transition to Dash state
        if (inputState.dash)
        {
            playerController.ChangeState(playerController.GetDashState());
            return true;
        }

        return false;
    }

    bool OnGroundState::CanAttack(PlayerController& playerController, const struct PlayerInputState& inputState)
    {
        // Transition to Attack state
        if (inputState.attack)
        {
            playerController.ChangeState(playerController.GetAttackState());
            return true;
        }

        return false;
    }
}