#include "Player/IdleState.h"
#include "Player/PlayerController.h"
#include "Player/MovingState.h"
#include "Input/PlayerInputManager.h"

namespace Game
{
    bool IdleState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
    {
        if (OnGroundState::HandleInput(playerController, inputState)) return true;
        if (CanJump(playerController, inputState)) return true;
        if (CanFall(playerController)) return true;
        if (CanDash(playerController, inputState)) return true;
        if (CanAttack(playerController, inputState)) return true;

        if (inputState.moveForward || inputState.moveBackward || inputState.moveLeft || inputState.moveRight)
        {
            // Transition to WalkState
            playerController.ChangeState(playerController.GetWalkingState());
            return true;
        }

        return false;
    }

    void IdleState::Enter(PlayerController& playerController)
    {
        // LOG_INFO("Entering Idle State");
        playerController.isCombat = false;
        playerController.GetAnimator()->PlayAnimation("Idle03");
    }

    void IdleState::OnUpdate(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState)
    {
        // Handle idle state logic here
    }

    void IdleState::Exit(PlayerController& playerController)
    {
        // LOG_INFO("Exiting Idle State");
    }
}