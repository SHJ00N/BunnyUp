#include "Player/PlayerAttackHitBox.h"
#include "Enemy/EnemyController.h"
#include "Player/PlayerController.h"
#include "Common/Health.h"
#include "Effect/EffectPoolManager.h"
#include "Common/ObjectPoolManager.h"

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

            auto* health = enemyController->GetHealth();
            if (!health) return;
            if (health->IsDeath()) return;

            enemyController->GetDamaged()->Damaged();
            health->TakeDamage(playerController->attackPower);
            if(health->IsDeath())
            {
                playerController->killCount++;

                 if (Random::Range(0, 3) == 0)
                {
                    ObjectPoolManager::GetInstance().GetPool("HeartItem", enemyController->ownerGameObject->transform.GetWorldPosition() + Vector3(0.0f, 10.0f, 0.0f), Vector3(0.0f), Vector3(0.05f));
                }
            }

            // effect
            const auto& transform = enemyController->ownerGameObject->transform;
            auto position = transform.GetWorldPosition();
            position.y += 5.0f;
            EffectPoolManager::GetInstance().GetPool(EffectObjectType::EnemyHit, position, Vector3(0.0f), transform.GetLocalScale() * 250.0f);
        }
    }
}