#include "Player/JumpAttackState.h"
#include "Player/PlayerController.h"
#include "Player/MovingState.h"
#include "Input/PlayerInputManager.h"
#include "Effect/EffectPoolManager.h"

namespace Game
{
    bool JumpAttackState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
    {
        if (OnAirState::HandleInput(playerController, inputState)) return true;

        if (Elapsed(0.2f))
        {
            if (IsGround(playerController, inputState)) return true;
        }

        return false;
    }

    void JumpAttackState::Enter(PlayerController& playerController)
    {
        // LOG_INFO("Entering JumpAttack State");
        playerController.GetAnimator()->PlayAnimation("Fall");

        auto rigidbody = playerController.GetRigidbody();
        auto forward = playerController.ownerGameObject->transform.GetForward();
        
        rigidbody->SetLinearVelocity(Vector3(0.0f, 0.0f, 0.0f));
        float impulse = playerController.jumpAttackImpulse;
        playerController.GetRigidbody()->AddImpulse(Vector3(forward.x * impulse, impulse, forward.z * impulse));

        const auto& transform = playerController.ownerGameObject->transform;
        Vector3 position = transform.GetWorldPosition();
        position.y += 7.5f;
        Quaternion rotation = transform.GetWorldRotationQuaternion();
        rotation = rotation * AngleAxis(-90.0f, transform.GetRight());
        EffectPoolManager::GetInstance().GetPool(EffectObjectType::Dust, position, QuaternionToEuler(rotation), Vector3(20.0f));
    }

    void JumpAttackState::Exit(PlayerController& playerController)
    {
        OnAirState::Exit(playerController);
        // LOG_INFO("Exiting JumpAttack State");
    }

    void JumpAttackState::OnUpdate(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState)
    {
        auto rigidbody = playerController.GetRigidbody();
        if (!playerController.isGrounded)
        {
            float gravityScale = rigidbody->GetLinearVelocity().y < 0.0f ? 25.0f : 15.0f;
            rigidbody->AddForce(Vector3(0.0f, rigidbody->GetMass() * -9.81f * gravityScale, 0.0f));
            ResetTimer();
        }
        else
        {
            rigidbody->SetLinearVelocity(Vector3(0.0f));
        }
    }
}