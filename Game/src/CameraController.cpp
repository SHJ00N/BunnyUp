#include "CameraController.h"
#include <GameObject.h>
#include <InputManager.h>
#include <algorithm>
#include <MathHelper.h>
#include <ImGuiClass.h>

void CameraController::Update(float dt)
{
	auto& input = Engine::InputManager::GetInstance();
	auto& transform = ownerGameObject->transform;

	if (input.IsMouseDown(Engine::MouseButton::Right))
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
	}
}

void CameraController::OnImGui()
{
	ImGui::DragFloat("Move Speed", &moveSpeed, 0.1f, 1.0f, 50.0f);
	ImGui::DragFloat("Sensitivity", &sensitivity, 0.01f, 0.1f, 1.0f);
}