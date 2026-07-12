#include "Player/AttackState.h"
#include "Player/PlayerController.h"
#include "Input/PlayerInputManager.h"

namespace Game
{
    bool AttackState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
    {
        // cancel attack
        if (Elapsed(0.7f))
        {
            if (CanJump(playerController, inputState)) return true;
            if (CanDash(playerController, inputState)) return true;
            if (inputState.attack)
            {
                playerController.ChangeState(playerController.GetAttackState());
                return true;
            }

            // moving state
            if (inputState.moveForward || inputState.moveBackward || inputState.moveLeft || inputState.moveRight)
            {
                // Transition to WalkState
                playerController.ChangeState(playerController.GetWalkingState());
                return true;
            }
        }

        // attack end
        if (Elapsed(0.8f))
        {
            playerController.ChangeState(playerController.GetIdleState());
            return true;
        }

        return false;
    }

    void AttackState::Enter(PlayerController& playerController)
    {
        LOG_INFO("Entering Attack state");
        playerController.GetAnimator()->PlayAnimation("Attack", true);
        ResetTimer();
    }

    void AttackState::Exit(PlayerController& playerController)
    {
        LOG_INFO("Exit Attack state");
    }

    void AttackState::OnUpdate(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState)
    {

    }
}