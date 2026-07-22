#pragma once

#include <EngineSystem.h>

namespace Game
{
    class HitBox : public Engine::Component
    {
    public:
        template<typename THitBox, typename... Args>
        static THitBox* CreateHitBox(
            Engine::GameObject* parent,
            const Engine::Vector3& center,
            const Engine::Vector3& size,
            const std::string& name,
            Args&&... args)
        {
            static_assert(std::is_base_of_v<HitBox, THitBox>, "THitBox must derive from HitBox");

            auto* obj = parent->scene->CreateChildGameObject<Engine::GameObject>(parent, name);

            auto* hitBox = obj->AddComponent<THitBox>(std::forward<Args>(args)...);

            auto* rigidbody = obj->AddComponent<Engine::Rp3dRigidbody>(1.0f, Engine::BodyType::KINEMATIC, false);

            auto* collider = obj->AddComponent<Engine::Rp3dBoxCollider>();
            collider->SetLocalPosition(center);
            collider->SetSize(size);
            collider->SetTrigger(true);

            hitBox->m_collider = collider;
            hitBox->m_owner = parent;
            return hitBox;
        }

        virtual ~HitBox() = default;

        void SetLayer(Engine::CollisionLayer layer) { }

        bool IsEnable() const { return m_isEnable; }
        void Enable() 
        { 
            m_isEnable = true; 
            m_hitTargets.clear(); 
        }
        void Disable() { m_isEnable = false; }

        void OnTriggerStay(Engine::Rp3dCollider* other) override = 0;

    protected:
        bool m_isEnable = false;
        Engine::Rp3dCollider* m_collider = nullptr;
        Engine::GameObject* m_owner = nullptr;
        std::unordered_set<Engine::GameObject*> m_hitTargets;
    };
}