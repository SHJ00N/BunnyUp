#include "Common/Health.h"

namespace Game
{
    Health::Health(int max) : m_maxHealth(max), m_health(max)
    {
    }

    void Health::TakeDamage(int damage)
    {
        m_health = std::max(0, m_health - damage);
        LOG_INFO("%s : %d", ownerGameObject->GetName().c_str(), m_health);
    }

    void Health::RecoveryHealth(int amount)
    {
        m_health = std::min(m_maxHealth, m_health + amount);
    }

    void Health::SetMaxHealth(int max)
    {
        m_maxHealth = max;
        m_health = std::min(m_health, m_maxHealth);
    };
}