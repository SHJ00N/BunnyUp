#pragma once

#include <EngineSystem.h>
#include "Enemy/EnemyState.h"

namespace Game
{
    class EnemyController;
    class EnemyDamaged : public EnemyState
    {
    public:
        // override function
        void Enter(EnemyController& controller) override;
        void Exit(EnemyController& controller) override;

        bool IsDamaged() const { return m_hasDamage; }
        void Damaged() { m_hasDamage = true; }

    protected:
        void OnUpdate(EnemyController& controller, float dt) override;

    private:
        bool m_hasDamage = false;
    };
}