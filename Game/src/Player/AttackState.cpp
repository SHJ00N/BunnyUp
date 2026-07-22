#include "Player/AttackState.h"
#include "Player/PlayerController.h"
#include "Input/PlayerInputManager.h"
#include "Player/PlayerAttackHitBox.h"

namespace Game
{
    void AttackState::Initialize(PlayerController& playerController)
    {
        m_hitBox = HitBox::CreateHitBox<PlayerAttackHitBox>(playerController.ownerGameObject, Vector3(0.0f, 7.5f, 8.0f), Vector3(15.0f, 15.0f, 15.0f), "AttackHitBox");
        m_hitBox->SetLayer(Engine::CollisionLayer::PlayerTrigger);
    }

    bool AttackState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
    {
        if (OnGroundState::HandleInput(playerController, inputState)) return true;

        // cancel attack
        if (Elapsed(0.7f))
        {
            if (CanJump(playerController, inputState)) return true;
            if (CanDash(playerController, inputState)) return true;
            if (inputState.attack)
            {
                playerController.ChangeState(playerController.GetAttackState(), true);
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
            playerController.ChangeState(playerController.GetCombatState());
            return true;
        }

        return false;
    }

    void AttackState::Enter(PlayerController& playerController)
    {
        // LOG_INFO("Entering Attack state");
        playerController.GetAnimator()->PlayAnimation("Attack", true);
        ResetTimer();
    }

    void AttackState::Exit(PlayerController& playerController)
    {
        // LOG_INFO("Exit Attack state");
        m_hitBox->Disable();
    }

    void AttackState::OnUpdate(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState)
    {
        if (Elapsed(0.2f) && !m_hitBox->IsEnable())
        {
            m_hitBox->Enable();
        }

        if (Elapsed(0.5f) && m_hitBox->IsEnable())
        {
            m_hitBox->Disable();
        }
    }
}