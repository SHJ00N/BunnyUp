#pragma once

#include <EngineSystem.h>

namespace Game
{
    class EnemyController;
    class EnemyState
    {
    public:
        virtual ~EnemyState() = default;

        void Update(EnemyController& controller, float dt)
        {
            m_elapsedTime += dt;
            OnUpdate(controller, dt);
        };
        virtual void Enter(EnemyController& controller) { }
        virtual void Exit(EnemyController& controller) { }

    protected:
        virtual void OnUpdate(EnemyController& controller, float dt) {}

        float m_elapsedTime = 0.0f;
        void ResetTimer() { m_elapsedTime = 0.0f; }
        bool Elapsed(float time) const { return m_elapsedTime >= time; }
    };
}