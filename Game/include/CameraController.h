#pragma once

#include <EngineSystem.h>

namespace Game
{
    class CameraController : public Engine::Component
    {
    public:
        void Update(float dt) override;
        void OnImGui() override;

        void SetTarget(Engine::GameObject* target);
        void SetCameraOffset(const Engine::Vector3& offset) { m_cameraOffset = offset; }
        void SetTargetOffset(const Engine::Vector3& offset) { m_targetOffset = offset; }
    private:
        Engine::GameObject* m_target = nullptr;
        Engine::Vector3 m_cameraOffset;
        Engine::Vector3 m_targetOffset;
    };
}