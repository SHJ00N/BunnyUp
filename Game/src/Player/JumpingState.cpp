#include "Player/JumpingState.h"
#include "Player/PlayerController.h"
#include "Player/MovingState.h"
#include "Input/PlayerInputManager.h"

namespace Game
{
    bool JumpingState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
    {
        if (OnAirState::HandleInput(playerController, inputState))
        {
            return true;
        }

        if (playerController.GetAnimator()->IsAnimationFinished() && playerController.GetRigidbody()->GetLinearVelocity().y <= -60.0f)
        {
            // if jumping animation is end, transition to falling state
            playerController.ChangeState(playerController.GetFallingState());
            return true;
        }

        return false;
    }

    void JumpingState::Enter(PlayerController& playerController)
    {
        LOG_INFO("Entering Jumping State");
        playerController.GetAnimator()->PlayAnimation("Jump_Place");
        playerController.GetRigidbody()->AddImpulse(Vector3(0.0f, 50.0f, 0.0f)); // Apply upward impulse for jump
        playerController.isGrounded = false;
    }

    void JumpingState::Exit(PlayerController& playerController)
    {
        OnAirState::Exit(playerController);
        LOG_INFO("Exiting Jumping State");
    }
}