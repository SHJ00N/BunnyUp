#include "Enemy/Thron.h"
#include "Player/PlayerController.h"
#include "Common/Health.h"
#include "Effect/EffectPoolManager.h"
#include "Common/ObjectPoolManager.h"

namespace Game
{
    using namespace Engine;

    Thron::Thron(int damage, float speed) : m_damage(damage), m_speed(speed)
    {
    }

    void Thron::Update(float dt)
    {
        auto& transform = ownerGameObject->transform;

        const Vector3 forward = transform.GetForward();

        auto position = transform.GetWorldPosition();
        position += forward * m_speed * dt;

        transform.SetLocalPosition(position);
    }

    void Thron::OnTriggerEnter(Engine::Rp3dCollider* other)
    {
        if (other->ownerGameObject->GetTag() == ObjectTag::Trigger) return;
        if (other->ownerGameObject->GetTag() == ObjectTag::Enemy) return;
        if (other->ownerGameObject->GetTag() == ObjectTag::EnemyHitBox) return;
        if (other->ownerGameObject->GetTag() == ObjectTag::PlayerHitBox) return;

        if (other->ownerGameObject->GetTag() == ObjectTag::Player)
        {
            auto* playerController = other->ownerGameObject->GetComponent<PlayerController>();
            if (!playerController)
            {
                return;
            }

            if (!playerController->isInvincible)
            {
                playerController->hasDamage = true;

                auto* playerHealth = playerController->GetHealth();
                if (!playerHealth) return;

                playerHealth->TakeDamage(m_damage);

                // effect
                const auto& transform = playerController->ownerGameObject->transform;
                auto position = transform.GetWorldPosition();
                position.y += 10.0f;
                EffectPoolManager::GetInstance().GetPool(EffectObjectType::PlayerHit, position, Vector3(0.0f), Vector3(25.0f));
            }
        }

        ObjectPoolManager::GetInstance().ReturnPool(ownerGameObject);
    }
}