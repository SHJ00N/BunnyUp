#include "Enemy/MeleeEnemyAttack.h"
#include "Enemy/EnemyController.h"
#include "Enemy/EnemyAttackHitBox.h"

namespace Game
{
    using namespace Engine;

    EnemyAttackHitBox* MeleeEnemyAttack::CreateHitBox(EnemyController& controller)
    {
        m_hitBox = HitBox::CreateHitBox<EnemyAttackHitBox>(controller.ownerGameObject, m_hitBoxCenter, m_hitBoxSize, "AttackHitBox");
        m_hitBox->SetLayer(Engine::CollisionLayer::EnemyTrigger);
        m_hitBox->ownerGameObject->SetTag(Engine::ObjectTag::EnemyHitBox);

        return m_hitBox;
    }

    void MeleeEnemyAttack::OnReset()
    {
        if (!m_hitBox) return;

        if (m_hitBox->IsEnable())
        {
            m_hitBox->Disable();
        }
    }

    void MeleeEnemyAttack::EnterAttack(EnemyController& controller)
    {
        auto direction = controller.GetTarget()->transform.GetWorldPosition() - controller.ownerGameObject->transform.GetWorldPosition();
        direction.y = 0.0f;
        direction = Normalize(direction);

        float yaw = Degrees(std::atan2(direction.x, direction.z));
        Quaternion rotation = RotationPitchYawRoll(0.0f, yaw, 0.0f);
        controller.ownerGameObject->transform.SetLocalRotation(rotation);

        // set animation to attack anim
        auto* animator = controller.GetAnimator();
        if (animator)
        {
            animator->PlayAnimation("Attack", true);
        }
    }

    void MeleeEnemyAttack::ExitAttack(EnemyController& controller)
    {
        if (m_hitBox->IsEnable())
        {
            m_hitBox->Disable();
        }
    }

    void MeleeEnemyAttack::StartAttack(EnemyController& controller)
    {
        m_hitBox->Enable();
    }

    void MeleeEnemyAttack::EndAttack(EnemyController& controller)
    {
        if (m_hitBox->IsEnable())
        {
            m_hitBox->Disable();
        }
    }
}