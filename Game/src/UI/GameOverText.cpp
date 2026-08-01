#include "UI/GameOverText.h"

namespace Game
{
    using namespace Engine;

    void GameOverText::OnStart()
    {
        m_textComponent = ownerGameObject->GetComponent<TextComponent>();

        m_alpha = 0.0f;
    }

    void GameOverText::OnEnable()
    {
        m_alpha = 0.0f;

        if (m_textComponent)
        {
            auto color = m_textComponent->GetColor();
            color.w = m_alpha;
            m_textComponent->SetColor(color);
        }
    }

    void GameOverText::FixedUpdate(float dt)
    {
        if (!m_textComponent) return;

        m_alpha = std::min(1.0f, m_alpha + m_fadeInSpeed * dt);
        
        auto color = m_textComponent->GetColor();
        color.w = m_alpha;
        m_textComponent->SetColor(color);
    }
}