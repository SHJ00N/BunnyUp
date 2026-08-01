#pragma once

#include <EngineSystem.h>

namespace Game
{
    class GameOverText : public Engine::Component
    {
    public:
        void FixedUpdate(float dt) override;

    protected:
        void OnStart() override;
        void OnEnable() override;

    private:
        Engine::TextComponent* m_textComponent = nullptr;

        float m_alpha = 0.0f;
        float m_fadeInSpeed = 0.5f;
    };
}