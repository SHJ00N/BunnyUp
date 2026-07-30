#pragma once

#include <EngineSystem.h>

namespace Game
{
    class Thron : public Engine::Component
    {
    public:
        Thron(int damage = 5, float speed = 10.0f);

        void SetSpeed(float speed) { m_speed = speed; }
        void SetDamage(int damage) { m_damage = damage; }

        void Update(float dt) override;
        void OnTriggerEnter(Engine::Rp3dCollider* other) override;

    private:
        int m_damage;
        float m_speed;
    };
}