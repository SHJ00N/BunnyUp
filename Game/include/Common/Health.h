#pragma once

#include <EngineSystem.h>

namespace Game
{
    class Health : public Engine::Component
    {
    public:
        Health(int max = 100);
        void TakeDamage(int damage);
        void RecoveryHealth(int amount);

        void SetMaxHealth(int max);

        bool IsDeath() const { return m_health <= 0; }

    private:
        int m_health;
        int m_maxHealth;
    };
}