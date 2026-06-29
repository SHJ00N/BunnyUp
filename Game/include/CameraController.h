#pragma once

#include <EngineSystem.h>

class CameraController : public Engine::Component
{
public:
	float moveSpeed = 10.0f;
	float sensitivity = 0.05f;
    float scrollSpeed = 0.05f;

	void Update(float dt) override;
	void OnImGui() override;
private:
	float yaw = 0.0f;
	float pitch = 0.0f;

	bool m_isActive = false;
};