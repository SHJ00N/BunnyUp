#pragma once

#include "Component.h"
#include "ConstantBuffer.h"

namespace Engine
{
	class Camera : public Component
	{
	public:
		float fov = 60.0f;
		float nearPlane = 0.1f;
		float farPlane = 2000.0f;
		bool frustumVisible = false;

		Matrix4x4 GetViewMatrix() const;
		Matrix4x4 GetProjectionMatrix() const;

		void UpdateConstantBuffer(class ConstantBufferManager& cbManager);

	protected:
		void OnAwake() override;
		void OnDestroy() override;

	private:
		ConstantBufferPerCamera m_cbPerCamera;
	};
}