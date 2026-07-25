#include "Effect/EffectPoolManager.h"

namespace Game
{
    using namespace Engine;

    EffectObject* EffectPoolManager::GetPool(EffectObjectType type, Engine::Vector3 position, Engine::Vector3 rotation, Engine::Vector3 scale)
    {
        auto& pool = m_pools[type];

        // Gett effect object
        EffectObject* effect = nullptr;
        if (pool.empty())
        {
            effect = createEffect(type);
        }
        else
        {
            effect = pool.front();
            pool.pop();
            effect->GetEffect()->Play();
        }
        // Set effect transform
        effect->ownerGameObject->transform.SetLocalPosition(position);
        effect->ownerGameObject->transform.SetLocalRotation(rotation);
        effect->ownerGameObject->transform.SetLocalScale(scale);
        // Set effect state
        effect->ownerGameObject->SetActive(true);

        return effect;
    }

    void EffectPoolManager::ReturnPool(EffectObject* object)
    {
        object->ownerGameObject->SetActive(false);
        m_pools[object->GetType()].push(object);
    }

    EffectObject* EffectPoolManager::createEffect(EffectObjectType type)
    {
        switch(type)
        {
        case EffectObjectType::PlayerHit:
        {
            auto* object =  ownerGameObject->CreateGameObject<GameObject>("PlayerHitEffect");
            auto* renderer = object->AddComponent<EffectRenderer>(EffectType::Billboard);
            renderer->SetSpriteSheet(ResourceManager::GetInstance().GetTexture("BHit_Effect"), 4, 4);
            auto* effect = object->AddComponent<EffectComponent>();

            return object->AddComponent<EffectObject>(type);
        }
        case EffectObjectType::EnemyHit:
        {
            auto* object = ownerGameObject->CreateGameObject<GameObject>("EnemyHitEffect");
            auto* renderer = object->AddComponent<EffectRenderer>(EffectType::Billboard);
            renderer->SetSpriteSheet(ResourceManager::GetInstance().GetTexture("AHit_Effect"), 4, 4);
            renderer->GetMaterial(0)->SetColor(Vector4(1.0f, 0.0f, 1.0f, 1.0f));
            auto* effect = object->AddComponent<EffectComponent>();

            return object->AddComponent<EffectObject>(type);
        }
        case EffectObjectType::Dust:
        {
            auto* object = ownerGameObject->CreateGameObject<GameObject>("Dust");
            auto* renderer = object->AddComponent<EffectRenderer>(EffectType::Normal);
            renderer->SetSpriteSheet(ResourceManager::GetInstance().GetTexture("Dust_Effect"), 5, 5);
            auto* effect = object->AddComponent<EffectComponent>();
            effect->SetFPS(45.0f);

            return object->AddComponent<EffectObject>(type);
        }
        }

        return nullptr;
    }
}