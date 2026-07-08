#include "CameraController.h"

namespace Game
{
    void CameraController::Update(float dt)
    {
        if (!m_target) return;

        auto& transform = ownerGameObject->transform;
        const auto& targetPosition = m_target->transform.GetWorldPosition();

        transform.SetLocalPosition(targetPosition + m_cameraOffset);
        transform.LookAt(targetPosition + m_targetOffset);
    }

    void CameraController::SetTarget(Engine::GameObject* target)
    {
        m_target = target;
    }

    void CameraController::OnImGui()
    {
        auto camera = ownerGameObject->GetComponent<Engine::Camera>();
        ImGui::Checkbox("Show Frustum", &camera->frustumVisible);

        ImGui::DragFloat3("Position offset", &m_cameraOffset.x, 0.01f);
        ImGui::DragFloat3("Target offset", &m_targetOffset.x, 0.01f);
    }
}