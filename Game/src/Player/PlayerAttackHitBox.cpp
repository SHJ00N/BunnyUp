#include "Player/PlayerAttackHitBox.h"
#include "Enemy/EnemyController.h"
#include "Player/PlayerController.h"
#include "Common/Health.h"
#include "Effect/EffectPoolManager.h"

namespace Game
{
    using namespace Engine;

    void PlayerAttackHitBox::OnTriggerStay(Engine::Rp3dCollider* other)
    {
        if (!m_isEnable) return;
        if (other->ownerGameObject == m_owner) return;

        if (other->ownerGameObject->GetTag() == ObjectTag::Enemy)
        {
            if (m_hitTargets.find(other->ownerGameObject) != m_hitTargets.end()) return;
            // add target
            m_hitTargets.insert(other->ownerGameObject);

            // trigger process
            auto* enemyController = other->ownerGameObject->GetComponent<EnemyController>();
            auto* playerController = m_owner->GetComponent<PlayerController>();
            if (!enemyController || !playerController) return;

            enemyController->GetDamaged()->Damaged();

            auto* health = enemyController->GetHealth();
            if (!health) return;

            health->TakeDamage(playerController->attackPower);

            // effect
            const auto& transform = enemyController->ownerGameObject->transform;
            auto position = transform.GetWorldPosition();
            position.y += 5.0f;
            EffectPoolManager::GetInstance().GetPool(EffectObjectType::EnemyHit, position, Vector3(0.0f), transform.GetLocalScale() * 250.0f);
        }
    }
}