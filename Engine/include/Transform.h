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
		const Vector3 GetForward() const;
		const Vector3 GetRight() const;
		const Vector3 GetUp() const;
		const Vector3 GetWorldPosition() const;
		const Vector3 GetWorldScale() const;
		const Vector3 GetLossyScale() const;
		const Matrix3x3 GetRotationMatrix() const;
		const Quaternion GetWorldRotationQuaternion() const;

		void SetLocalPosition(const Vector3& position);
		void SetLocalRotation(const Vector3& rotation);
		void SetLocalRotation(const Quaternion& rotation);
		void SetLocalScale(const Vector3& scale);
		void SetDirty() { m_isDirty = true; }
		void LookAt(const Vector3& target);
		void LookAt(const Vector3& target, const Vector3& up);
		void SetWorldMatrix(const Matrix4x4& world);

		const Vector3& GetLocalPosition() const { return m_position; }
		Vector3 GetLocalEulerRotation() const { return QuaternionToEuler(m_quaternionRotation); }
		const Quaternion& GetLocalQuaternionRotation() const { return m_quaternionRotation; }
		const Vector3& GetLocalScale() const { return m_scale; }
		const bool IsDirty() const { return m_isDirty; }

	private:
		// Local space information
		Vector3 m_position;
		Quaternion m_quaternionRotation;
		Vector3 m_scale;

		// Global space information
		mutable Matrix4x4 m_worldMatrix;

		// dirty flag
		mutable bool m_isDirty;

		Matrix4x4 calculateLocalMatrix() const;
		void updateWorldMatrix() const;
	};
}