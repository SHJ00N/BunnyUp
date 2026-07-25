#include "Enemy/EnemyAttackHitBox.h"
#include "Player/PlayerController.h"
#include "Enemy/EnemyController.h"
#include "Common/Health.h"
#include "Effect/EffectPoolManager.h"

namespace Game
{
    using namespace Engine;
    
    void EnemyAttackHitBox::OnTriggerStay(Engine::Rp3dCollider* other)
    {
        if (!m_isEnable) return;
        if (other->ownerGameObject == m_owner) return;

        if (other->ownerGameObject->GetTag() == ObjectTag::Player)
        {
            if (m_hitTargets.find(other->ownerGameObject) != m_hitTargets.end()) return;

            m_hitTargets.insert(other->ownerGameObject);
            
            auto* enemyController = m_owner->GetComponent<EnemyController>();
            auto* playerController = other->ownerGameObject->GetComponent<PlayerController>();
            if (!enemyController || !playerController)
            {
                return;
            }

            if (!playerController->isInvincible)
            {
                playerController->hasDamage = true;

                auto* playerHealth = playerController->GetHealth();
                if (!playerHealth) return;
                
                playerHealth->TakeDamage(enemyController->attackPower);

                // effect
                const auto& transform = playerController->ownerGameObject->transform;
                auto position = transform.GetWorldPosition();
                position.y += 10.0f;
                EffectPoolManager::GetInstance().GetPool(EffectObjectType::PlayerHit, position, Vector3(0.0f), Vector3(25.0f));
            }
        }
    }
}