#include "pch.h"
#include "EditorCameraController.h"
#include "InputManager.h"
#include "algorithm"
#include "MathHelper.h"
#include "ImGuiClass.h"
#include "Camera.h"
#include "GameObject.h"

namespace Engine
{
	void EditorCameraController::Update(float dt)
	{
		auto& input = Engine::InputManager::GetInstance();
		auto& transform = ownerGameObject->transform;

		if (input.IsEditorMode())
		{
			// set mouse position releative mode
			if (input.IsMousePressed(MouseButton::Wheel))
			{
				input.OnFPSMode();
			}

			// set mouse position absolute mode
			if (input.IsMouseReleased(MouseButton::Wheel))
			{
				input.OffFPSMode();
			}

			if (input.isFPSMode())
			{
				// set rotation from mouse movement
				float dx = (float)input.GetMouseDeltaX();
				float dy = (float)input.GetMouseDeltaY();

				yaw += dx * mouseSensitivity;
				pitch += dy * mouseSensitivity;

				// Prevents gimbal lock by limiting pitch.
				pitch = std::clamp(pitch, -89.0f, 89.0f);

				transform.SetLocalRotation(Engine::Vector3(pitch, yaw, 0.0f));
			}

			// set position from keyboard input
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
		}
		else
		{
			// set mouse position absolute mode
			if (input.isFPSMode())
			{
				input.OffFPSMode();
			}
		}
	}

	void EditorCameraController::OnImGui()
	{
		ImGui::DragFloat("Move Speed", &moveSpeed, 0.1f, 1.0f, 50.0f);
		ImGui::DragFloat("Sensitivity", &mouseSensitivity, 0.01f, 0.01f, 0.1f);
		ImGui::DragFloat("Field of View", &ownerGameObject->GetComponent<Camera>()->fov, 1.0f, 1.0f, 179.0f);

		auto camera = ownerGameObject->GetComponent<Camera>();
		ImGui::Checkbox("Show Frustum", &camera->frustumVisible);
	}
}