#pragma once

#include <Component.h>

class CameraController : public Engine::Component
{
public:
	float moveSpeed = 5.0f;
	float sensitivity = 0.1f;

	void Update(float dt) override;
	void OnImGui() override;
private:
	float yaw = 0.0f;
	float pitch = 0.0f;
};