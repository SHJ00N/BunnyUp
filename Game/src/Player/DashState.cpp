#include "Player/DashState.h"
#include "Player/PlayerController.h"
#include "Input/PlayerInputManager.h"
#include "Effect/EffectPoolManager.h"

namespace Game
{
    bool DashState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
    {
        if (OnGroundState::HandleInput(playerController, inputState)) return true;

        if (Elapsed(0.5f) && CanJump(playerController, inputState)) return true;
        
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
            if (playerController.isCombat)
            {
                playerController.ChangeState(playerController.GetCombatState());
            }
            else
            {
                playerController.ChangeState(playerController.GetIdleState());
            }

            return true;
        }

        return false;
    }

    void DashState::Enter(PlayerController& playerController)
    {
        // LOG_INFO("Entering Dash state");
        playerController.isInvincible = true;
        playerController.GetAnimator()->PlayAnimation("Dash", true);
        ResetTimer();

        m_direction = playerController.ownerGameObject->transform.GetForward();

        const auto& transform = playerController.ownerGameObject->transform;
        Vector3 position = transform.GetWorldPosition();
        position.y += 7.5f;
        Quaternion rotation = transform.GetWorldRotationQuaternion();
        rotation = rotation * AngleAxis(-90.0f, transform.GetRight());
        EffectPoolManager::GetInstance().GetPool(EffectObjectType::Dust, position, QuaternionToEuler(rotation), Vector3(20.0f));
    }

    void DashState::Exit(PlayerController& playerController)
    {
        // LOG_INFO("Exit Dash state");
        auto rigidbody = playerController.GetRigidbody();
        auto velocity = rigidbody->GetLinearVelocity();
        rigidbody->SetLinearVelocity(Vector3(0.0f, velocity.y, 0.0f));
        rigidbody->SetLinearDamping(PlayerDefaultDamping);
    }

    void DashState::OnUpdate(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState)
    {
        // dash process
        if (Elapsed(0.2f))
        {
            playerController.isInvincible = false;
        }

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