#pragma once

#include "MathHelper.h"

namespace Engine
{
	class Transform
	{
	public:
		class GameObject* owner;
		Transform* parentTransform; // Pointer to parent transform, nullptr if root

		Transform();
		~Transform() = default;

		const Matrix4x4& GetWorldMatrix() const;

		void SetLocalPosition(const Vector3& position);
		void SetLocalRotation(const Vector3& rotation);
		void SetLocalScale(const Vector3& scale);
		void SetDirty() { m_isDirty = true; }

		const Vector3& GetLocalPosition() const { return m_position; }
		Vector3& GetLocalRotation() { return m_rotation; }
		const Vector3& GetLocalScale() const { return m_scale; }
		const bool IsDirty() const { return m_isDirty; }

	private:
		// Local space information
		Vector3 m_position;
		Vector3 m_rotation; // Euler angles in degrees
		Vector3 m_scale;

		// Global space information
		mutable Matrix4x4 m_worldMatrix;

		// dirty flag
		mutable bool m_isDirty;

		Matrix4x4 calculateLocalMatrix() const;
		void updateWorldMatrix() const;
	};
}