#include "Enemy/EnemyAttack.h"
#include "Enemy/EnemyController.h"
#include "Enemy/EnemyAttackHitBox.h"

namespace Game
{
    using namespace Engine;

    void EnemyAttack::Initialize(EnemyController& controller)
    {
        m_hitBox = HitBox::CreateHitBox<EnemyAttackHitBox>(controller.ownerGameObject, Vector3(0.0f, 4.0f, 6.0f), Vector3(11.0f, 7.5f, 10.0f), "AttackHitBox");
        m_hitBox->SetLayer(Engine::CollisionLayer::EnemyTrigger);
    }

    bool EnemyAttack::IsTargetInRange(EnemyController& controller) const 
    {
        auto* target = controller.GetTarget();
        if (!target)
        {
            return false;
        }

        const Vector3 targetPos = target->transform.GetWorldPosition();
        const Vector3 enemyPos = controller.ownerGameObject->transform.GetWorldPosition();
        return LengthSq(targetPos - enemyPos) <= m_attackRange * m_attackRange;
    }

    bool EnemyAttack::CanAttack() const
    {
        return m_attackCoolTimer <= 0.0f;
    }

    bool EnemyAttack::IsAttacking() const
    {
        return m_isAttacking;
    }

    void EnemyAttack::Enter(EnemyController& controller)
    {
        m_isAttacking = true;
        m_isAttacked = false;
        m_attackCoolTimer = m_attackCoolTime;
        m_attackTimer = 0.0f;
        
        // set animation to attack anim
        auto* animator = controller.GetAnimator();
        if (animator)
        {
            animator->PlayAnimation("Attack", true);
        }
    }

    void EnemyAttack::Exit(EnemyController& controller)
    {
        m_isAttacking = false;
        
        m_attackTimer = 0.0f;
        m_hitBox->Disable();
    }

    void EnemyAttack::OnUpdate(EnemyController& controller, float dt)
    {
        if (!m_isAttacking) return;

        m_attackTimer += dt;
        if (m_attackTimer >= 0.3f && !m_isAttacked)
        {
            m_isAttacked = true;
            m_hitBox->Enable();
        }

        if (m_attackTimer >= 0.6f && m_isAttacked)
        {
            if (m_hitBox->IsEnable())
            {
                m_hitBox->Disable();
            }
        }

        auto* animator = controller.GetAnimator();
        if (animator->IsAnimationFinished())
        {
            m_isAttacking = false;
        }
    }

    void EnemyAttack::UpdateTimer(float dt)
    {
        if (m_attackCoolTimer > 0.0f)
        {
            m_attackCoolTimer = std::max(0.0f, m_attackCoolTimer - dt);
        }
    }

    void EnemyAttack::DebugDraw(EnemyController & controller) const
    {
        // draw attack range for debugging
        DebugRenderer::GetInstance().AddSphere(controller.ownerGameObject->transform.GetWorldPosition(), m_attackRange, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    void EnemyAttack::EditorGui()
    {
        ImGui::DragFloat("AttackRange", &m_attackRange, 0.1f, 1.0f, 100.0f);
        ImGui::DragFloat("AttackCoolTime", &m_attackCoolTime, 0.1f, 1.0f, 100.0f);
    }
}