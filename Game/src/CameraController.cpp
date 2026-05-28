#include "CameraController.h"
#include <GameObject.h>
#include <InputManager.h>
#include <algorithm>
#include <MathHelper.h>
#include <ImGuiClass.h>
#include <Camera.h>
#include <Scene.h>

void CameraController::Update(float dt)
{
	auto& input = Engine::InputManager::GetInstance();
	auto& transform = ownerGameObject->transform;

    if (input.IsEditorMode())
    {
        if (m_isActive)
        {
            m_isActive = false;
        }
        return;
    }

	if (!m_isActive && input.IsKeyPressed(DirectX::Keyboard::Keys::F2))
	{
		m_isActive = true;
        input.OnFPSMode();

        // set main camera
        auto scene = ownerGameObject->scene;
        auto camera = ownerGameObject->GetComponent<Engine::Camera>();
        if (scene->GetMainCamera() != camera)
        {
            ownerGameObject->scene->SetMainCamera(camera);
        }
	}

	if (m_isActive)
	{
		float dx = (float)input.GetMouseDeltaX();
		float dy = (float)input.GetMouseDeltaY();

		yaw += dx * sensitivity;
		pitch += dy * sensitivity;

		// Prevents gimbal lock by limiting pitch.
		pitch = std::clamp(pitch, -89.0f, 89.0f);

		transform.SetLocalRotation(Engine::Vector3(pitch, yaw, 0.0f));

		Engine::Vector3 forward = transform.GetForward();
		Engine::Vector3 right = transform.GetRight();

		Engine::Vector3 pos = transform.GetLocalPosition();

		if (input.IsKeyDown(DirectX::Keyboard::Keys::W))
			pos += forward * moveSpeed * dt;

		if (input.IsKeyDown(DirectX::Keyboard::Keys::S))
			pos -= forward * moveSpeed * dt;

		if (input.IsKeyDown(DirectX::Keyboard::Keys::A))
			pos -= right * moveSpeed * dt;

		if (input.IsKeyDown(DirectX::Keyboard::Keys::D))
			pos += right * moveSpeed * dt;

		// camera move up and down
		if (input.IsKeyDown(DirectX::Keyboard::Keys::E))
			pos.y += moveSpeed * dt;

		if (input.IsKeyDown(DirectX::Keyboard::Keys::Q))
			pos.y -= moveSpeed * dt;

		transform.SetLocalPosition(pos);

        // set fov from mouse scrolling
        auto camera = ownerGameObject->GetComponent<Engine::Camera>();
        camera->fov -= input.GetScrollDelta() * scrollSpeed;
        camera->fov = std::clamp(camera->fov, 45.0f, 90.0f);
	}
}

void CameraController::OnImGui()
{
	ImGui::DragFloat("Move Speed", &moveSpeed, 0.1f, 1.0f, 50.0f);
	ImGui::DragFloat("Sensitivity", &sensitivity, 0.01f, 0.01f, 0.1f);
    ImGui::DragFloat("Scroll Speed", &scrollSpeed, 0.01f, 0.1f, 0.01f);

    auto camera = ownerGameObject->GetComponent<Engine::Camera>();
    ImGui::Checkbox("Show Frustum", &camera->frustumVisible);
}