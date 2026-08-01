#pragma once

#include <EngineSystem.h>

namespace Game
{
    class HeartItem : public Engine::Component
    {
    public:
        int recoveryAmount = 20;
        void Update(float dt) override;
        void OnTriggerEnter(Engine::Rp3dCollider* other) override;

    protected:
        void OnStart() override;
        void OnEnable() override;   

    private:
        Engine::Vector3 m_initialPosition;
        float m_time = 0.0f;
    };
}