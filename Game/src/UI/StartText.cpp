#include "UI/StartText.h"

namespace Game
{
    using namespace Engine;

    void StartText::OnStart()
    {
        m_textComponent = ownerGameObject->GetComponent<TextComponent>();

        m_alpha = 1.0f;
    }

    void StartText::OnEnable()
    {
        m_alpha = 1.0f;

        if (m_textComponent)
        {
            auto color = m_textComponent->GetColor();
            color.w = m_alpha;
            m_textComponent->SetColor(color);
        }
    }

    void StartText::FixedUpdate(float dt)
    {
        if (!m_textComponent) return;

        if (m_isFadingIn)
        {
            m_alpha = std::min(1.0f, m_alpha + m_fadeInSpeed * dt);
        }
        else
        {
            m_alpha = std::max(0.0f, m_alpha - m_fadeInSpeed * dt);
        }

        if (m_alpha >= 1.0f || m_alpha <= 0.0f)
        {
            m_isFadingIn = !m_isFadingIn;
        }
        
        auto color = m_textComponent->GetColor();
        color.w = m_alpha;
        m_textComponent->SetColor(color);
    }
}