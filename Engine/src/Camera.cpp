#include "Camera.h"
#include "ConstantBufferManager.h"
#include "D3DManager.h"
#include "GameObject.h"
#include "Scene.h"

namespace Engine
{
	void Camera::OnAwake()
	{
		ownerGameObject->scene->RegisterCamera(this);
	}

	void Camera::OnDestroy()
	{
		ownerGameObject->scene->UnregisterCamera(this);
	}

	Matrix4x4 Camera::GetViewMatrix() const
	{
		return Inverse(ownerGameObject->transform.GetWorldMatrix());
	}

	Matrix4x4 Camera::GetProjectionMatrix() const
	{
		return PerspectiveFovLH(Radians(fov), D3DManager::GetInstance().GetAspectRatio(), nearPlane, farPlane);
	}

	void Camera::UpdateConstantBuffer(ConstantBufferManager& cbManager)
	{
		Vector3 position = ownerGameObject->transform.GetWorldPosition();
		m_cbPerCamera.position = Vector4(position.x, position.y, position.z, 1.0f);
		m_cbPerCamera.view = GetViewMatrix();
		m_cbPerCamera.projection = GetProjectionMatrix();
		m_cbPerCamera.invView = Inverse(m_cbPerCamera.view);
		m_cbPerCamera.invProjection = Inverse(m_cbPerCamera.projection);
		cbManager.UpdatePerCamera(m_cbPerCamera);
	}
}