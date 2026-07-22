#include "Player/CombatState.h"
#include "Player/PlayerController.h"
#include "Player/MovingState.h"
#include "Input/PlayerInputManager.h"

namespace Game
{
    bool CombatState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
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

        if (Elapsed(5.0f))
        {
            playerController.ChangeState(playerController.GetIdleState());
        }

        return false;
    }

    void CombatState::Enter(PlayerController& playerController)
    {
        // LOG_INFO("Entering combat State");
        playerController.isCombat = true;
        playerController.GetAnimator()->PlayAnimation("IdleB");
        ResetTimer();
    }

    void CombatState::Exit(PlayerController& playerController)
    {
        // LOG_INFO("Exiting combat State");
    }
}