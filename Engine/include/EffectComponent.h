#pragma once

#include "Component.h"

namespace Engine
{
    class EffectRenderer;
	class EffectComponent : public Component
	{
    public:
        void Update(float dt) override;

        void Play();
        void Stop();

        void SetFPS(float fps) { m_fps = fps; }
        void SetLoop(bool value) { m_isLoop = value; }
        
        bool IsPlaying() const { return m_isPlaying; }
        bool IsFinish() const { return m_isFinish; }

        void OnImGui() override;

    protected:
        void OnStart() override;

    private:
        EffectRenderer* m_effectRenderer = nullptr;

        float m_fps = 30.0f;
        float m_timer = 0.0f;

        uint32_t m_frame = 0;
        bool m_isLoop = false;

        bool m_isPlaying = false;
        bool m_isFinish = false;
	};
}