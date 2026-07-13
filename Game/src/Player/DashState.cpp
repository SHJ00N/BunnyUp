#include "Player/DashState.h"
#include "Player/PlayerController.h"
#include "Input/PlayerInputManager.h"

namespace Game
{
    bool DashState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
    {
        if (Elapsed(0.1f) && CanJump(playerController, inputState)) return true;
        
        // cancel dash
        if (Elapsed(0.7f))
        {
            if (inputState.dash)
            {
                playerController.ChangeState(playerController.GetDashState(), true);
                return true;
            }
            if (CanAttack(playerController, inputState)) return true;

            // moving state
            if (inputState.moveForward || inputState.moveBackward || inputState.moveLeft || inputState.moveRight)
            {
                // Transition to WalkState
                playerController.ChangeState(playerController.GetWalkingState());
                return true;
            }
        }

        // dash end
        if (Elapsed(1.0f))
        {
            playerController.ChangeState(playerController.GetIdleState());
            return true;
        }

        return false;
    }

    void DashState::Enter(PlayerController& playerController)
    {
        LOG_INFO("Entering Dash state");
        playerController.GetAnimator()->PlayAnimation("Dash", true);
        ResetTimer();

        m_direction = playerController.ownerGameObject->transform.GetForward();
    }

    void DashState::Exit(PlayerController& playerController)
    {
        LOG_INFO("Exit Dash state");
        auto rigidbody = playerController.GetRigidbody();
        auto velocity = rigidbody->GetLinearVelocity();
        rigidbody->SetLinearVelocity(Vector3(0.0f, velocity.y, 0.0f));
        rigidbody->SetLinearDamping(PlayerDefaultDamping);
    }

    void DashState::OnUpdate(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState)
    {
        // dash process
        auto rigidbody = playerController.GetRigidbody();
        if (!Elapsed(0.3f))
        {
            auto velocity = rigidbody->GetLinearVelocity();

            velocity.x = m_direction.x * playerController.dashSpeed;
            velocity.z = m_direction.z * playerController.dashSpeed;

            rigidbody->SetLinearVelocity(velocity);
        }
        else
        {
            rigidbody->SetLinearDamping(10.0f);
        }
    }
}