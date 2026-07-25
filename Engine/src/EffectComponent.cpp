#include "pch.h"
#include "EffectComponent.h"
#include "EffectRenderer.h"
#include "ImGuiClass.h"

namespace Engine
{
    void EffectComponent::Play()
    { 
        m_isFinish = false;
        m_isPlaying = true;
        m_timer = 0.0f;
        m_frame = 0;
    }

    void EffectComponent::Stop()
    { 
        m_isPlaying = false;
        m_isFinish = true;
    }

    void EffectComponent::OnStart()
    {
        m_effectRenderer = ownerGameObject->GetComponent<EffectRenderer>();
    }

    void EffectComponent::Update(float dt)
    {
        if (!m_isPlaying) return;
        if (!m_effectRenderer) return;

        m_timer += dt;

        float frameDuration = 1.0f / m_fps;
        if (m_timer >= frameDuration)
        {
            m_timer -= frameDuration;
            ++m_frame;
        }

        const auto frameCount = m_effectRenderer->GetFrameCount();
        if (m_frame >= frameCount)
        {
            if (m_isLoop)
            {
                m_frame = 0;
            }
            else
            {
                Stop();
            }
        }
        
        m_effectRenderer->SetCurrentFrame(std::min(m_frame, frameCount - 1));
    }

    void EffectComponent::OnImGui()
    {
        if (ImGui::Checkbox("Loop", &m_isLoop))
        {
            Play();
        }

        ImGui::DragFloat("FPS", &m_fps);
    }
}