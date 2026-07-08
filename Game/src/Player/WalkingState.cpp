#include "Player/WalkingState.h"
#include "Player/PlayerController.h"
#include "Input/PlayerInputManager.h"

namespace Game
{
    bool WalkingState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
    {
        if(MovingState::HandleInput(playerController, inputState))
        {
            return true;
        }

        if (inputState.run)
        {
            // Transition to RunningState
            playerController.ChangeState(playerController.GetRunningState());
            return true;
        }

        return false;
    }

    void WalkingState::Enter(PlayerController& playerController)
    {
        LOG_INFO("Entering Walking State");
        playerController.GetAnimator()->PlayAnimation("Walk");
        playerController.isRunning = false;
    }

    void WalkingState::Exit(PlayerController& playerController)
    {
        MovingState::Exit(playerController);
        LOG_INFO("Exiting Walking State");
    }
}