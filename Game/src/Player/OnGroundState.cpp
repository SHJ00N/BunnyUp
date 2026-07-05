#include "Player/PlayerController.h"
#include "Input/PlayerInputManager.h"
#include "Player/OnGroundState.h"

namespace Game
{
    bool OnGroundState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
    {
        // Transition to Jumping state
        if (inputState.jump && playerController.CanJump())
        {
            playerController.ConsumeJump();
            playerController.ChangeState(playerController.GetJumpingState());
            return true;
        }

        // Transition to Falling state
        if (!playerController.isGrounded)
        {
            playerController.ChangeState(playerController.GetFallingState());
            return true;
        }

        return false;
    }
}