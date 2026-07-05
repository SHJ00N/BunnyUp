#include "Player/RunningState.h"
#include "Player/PlayerController.h"
#include "Input/PlayerInputManager.h"

namespace Game
{
    bool RunningState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
    {
        if (MovingState::HandleInput(playerController, inputState))
        {
            return true;
        }

        if (!inputState.run)
        {
            // Transition to WalkingState
            playerController.ChangeState(playerController.GetWalkingState());
            return true;
        }

        return false;
    }

    void RunningState::Enter(PlayerController& playerController)
    {
        LOG_INFO("Entering Running State");
        playerController.GetAnimator()->PlayAnimation("Run");
        playerController.moveSpeed = 40.0f; // Set running speed
    }

    void RunningState::Exit(PlayerController& playerController)
    {
        MovingState::Exit(playerController);
        LOG_INFO("Exiting Running State");
    }
}