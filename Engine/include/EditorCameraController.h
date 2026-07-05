#pragma once

#include "Component.h"

namespace Engine
{
	class EditorCameraController : public Component
	{
	public:
		float moveSpeed = 25.0f;
		float mouseSensitivity = 0.05f;

		void Update(float dt) override;
		void OnImGui() override;
	private:
		float yaw = 0.0f;
		float pitch = 0.0f;
	};
}