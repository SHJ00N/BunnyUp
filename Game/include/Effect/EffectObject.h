#pragma once

#include <EngineSystem.h>

namespace Game
{
    enum class EffectObjectType
    {
        PlayerHit,
        EnemyHit,
        Dust
    };

    class EffectObject : public Engine::Component
    {
    public:
        EffectObject(EffectObjectType type);

        void Update(float dt) override;

        Engine::EffectComponent* GetEffect() const { return m_effect; }
        const EffectObjectType GetType() const { return m_type; }
    protected:
        void OnStart() override;

    private:
        Engine::EffectComponent* m_effect = nullptr;
        EffectObjectType m_type;
    };
}