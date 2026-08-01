#include "HeartItem.h"
#include "Player/PlayerController.h"
#include "Common/Health.h"
#include "Effect/EffectPoolManager.h"
#include "Common/ObjectPoolManager.h"

namespace Game
{
    using namespace Engine;

    void HeartItem::OnStart()
    {
        m_initialPosition = ownerGameObject->transform.GetLocalPosition();
    }

    void HeartItem::OnEnable()
    {
        m_initialPosition = ownerGameObject->transform.GetLocalPosition();
    }

    void HeartItem::Update(float dt)
    {
        m_time += dt;

        constexpr float amplitude = 0.75f; // move height
        constexpr float speed = 2.0f;     // move speed

        Vector3 position = m_initialPosition;
        position.y += std::sin(m_time * speed) * amplitude;

        ownerGameObject->transform.SetLocalPosition(position);
    }

    void HeartItem::OnTriggerEnter(Engine::Rp3dCollider* other)
    {
        if (other->ownerGameObject->GetTag() == Engine::ObjectTag::Player)
        {
            auto* playerController = other->ownerGameObject->GetComponent<PlayerController>();
            if (!playerController)
            {
                return;
            }

            auto* health = other->ownerGameObject->GetComponent<Health>();
            if (health)
            {
                health->RecoveryHealth(recoveryAmount);
                // effect
                const auto& transform = other->ownerGameObject->transform;
                EffectPoolManager::GetInstance().GetPool(EffectObjectType::Heal, transform.GetWorldPosition() + Vector3(0.0f, 10.0f, 0.0f), Vector3(0.0f), Vector3(10.0f));

                ObjectPoolManager::GetInstance().ReturnPool(ownerGameObject);
            }
        }
    }
}