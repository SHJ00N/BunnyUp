#include "pch.h"
#include "Log.h"
#include "Transform.h"
#include "GameObject.h"
#include "Rigidbody_Legacy.h"

namespace Engine
{
	Transform::Transform() : owner(nullptr), parentTransform(nullptr), m_position(0, 0, 0), m_scale(1.0f, 1.0f, 1.0f), m_worldMatrix(Matrix4x4::Identity()), m_isDirty(true)
	{
	}

	const Matrix4x4& Transform::GetWorldMatrix() const
	{
		if (m_isDirty)
		{
			updateWorldMatrix();
		}
		return m_worldMatrix;
	}

	const Vector3 Transform::GetForward() const
	{
		auto& worldMatrix = GetWorldMatrix();
		return Normalize(Vector3(worldMatrix.m20, worldMatrix.m21, worldMatrix.m22));
	}

	const Vector3 Transform::GetRight() const
	{
		auto& worldMatrix = GetWorldMatrix();
		return Normalize(Vector3(worldMatrix.m00, worldMatrix.m01, worldMatrix.m02));
	}

	const Vector3 Transform::GetUp() const
	{
		auto& worldMatrix = GetWorldMatrix();
		return Normalize(Vector3(worldMatrix.m10, worldMatrix.m11, worldMatrix.m12));
	}

	const Vector3 Transform::GetWorldPosition() const
	{
		auto& worldMatrix = GetWorldMatrix();
		return Vector3(worldMatrix.m30, worldMatrix.m31, worldMatrix.m32);
	}

	const Vector3 Transform::GetLossyScale() const
	{
		const auto& world = GetWorldMatrix();

		return
		{
			Length(Vector3(world.m00, world.m01, world.m02)),
			Length(Vector3(world.m10, world.m11, world.m12)),
			Length(Vector3(world.m20, world.m21, world.m22))
		};
	}

	const Matrix3x3 Transform::GetRotationMatrix() const
	{
		Matrix3x3 result;

		Vector3 right = GetRight();
		Vector3 up = GetUp();
		Vector3 forward = GetForward();

		result.m[0][0] = right.x;
		result.m[0][1] = right.y;
		result.m[0][2] = right.z;

		result.m[1][0] = up.x;
		result.m[1][1] = up.y;
		result.m[1][2] = up.z;

		result.m[2][0] = forward.x;
		result.m[2][1] = forward.y;
		result.m[2][2] = forward.z;

		return result;
	}

	const Quaternion Transform::GetWorldRotationQuaternion() const
	{
		if (parentTransform)
		{
			return Normalize(m_quaternionRotation * parentTransform->GetWorldRotationQuaternion());
		}

		return m_quaternionRotation;
	}

	void Transform::updateWorldMatrix() const
	{
		if (parentTransform)
			m_worldMatrix = calculateLocalMatrix() * parentTransform->GetWorldMatrix();
		else
			m_worldMatrix = calculateLocalMatrix();

		m_isDirty = false;
	}

	Matrix4x4 Transform::calculateLocalMatrix() const
	{
		// translation
		Matrix4x4 translation = Translation(m_position);

		// rotation
		Matrix4x4 rotation = Rotation(m_quaternionRotation);

		// scaling
		Matrix4x4 scaling = Scaling(m_scale);

		// combine transformations
		return  scaling * rotation * translation; // Note: SRT order (Scaling * Rotation * Tranlsation)
	}

	void Transform::SetLocalPosition(const Vector3& position)
	{
		m_position = position;
		SetDirty();

		// Propagate dirty flag to children
		if (owner)
		{
			owner->PropagateTransformDirtyFlag();
		}
	}

	void Transform::SetLocalRotation(const Vector3& rotation)
	{
		m_quaternionRotation = RotationPitchYawRoll(rotation.x, rotation.y, rotation.z);
		SetDirty();

		// Propagate dirty flag to children
		if (owner)
		{
			owner->PropagateTransformDirtyFlag();
		}
	}

	void Transform::SetLocalRotation(const Quaternion& rotation)
	{
		m_quaternionRotation = Normalize(rotation);
		SetDirty();

		// Propagate dirty flag to children
		if (owner)
		{
			owner->PropagateTransformDirtyFlag();
		}
	}

	void Transform::SetLocalScale(const Vector3& scale)
	{
		m_scale = scale;
		SetDirty();
		
		// Propagate dirty flag to children
		if (owner)
		{
			owner->PropagateTransformDirtyFlag();

			auto rigidbody = owner->GetComponent<Rigidbody>();
			if (rigidbody)
			{
				rigidbody->SetDirty();
			}
		}
	}

	void Transform::LookAt(const Vector3& target)
	{
		LookAt(target, Vector3(0.f, 1.f, 0.f));
	}

	void Transform::LookAt(const Vector3& target, const Vector3& up)
	{
		Vector3 position = GetWorldPosition();

		if (LengthSq(target - position) < 1e-6f)
			return;

		Matrix4x4 view = LookAtLH(position, target, up);
		Matrix4x4 world = Inverse(view);

		Quaternion worldRotation = RotationQuaternion(world);

		if (parentTransform)
		{
			Quaternion local = Inverse(parentTransform->GetWorldRotationQuaternion()) * worldRotation;
			SetLocalRotation(local);
		}
		else
		{
			SetLocalRotation(worldRotation);
		}
	}

	void Transform::SetWorldMatrix(const Matrix4x4& world)
	{
		m_worldMatrix = world;
		SetDirty();
	}
}