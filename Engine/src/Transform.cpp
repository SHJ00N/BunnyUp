#include "Transform.h"
#include "GameObject.h"

namespace Engine
{
	Transform::Transform() : owner(nullptr), parentTransform(nullptr), m_position(0, 0, 0), m_rotation(0, 0, 0), m_scale(1.0f, 1.0f, 1.0f), m_worldMatrix(Matrix4x4::Identity()), m_isDirty(true)
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

	void Transform::updateWorldMatrix() const
	{
		if (parentTransform)
			m_worldMatrix = parentTransform->GetWorldMatrix() * calculateLocalMatrix();
		else
			m_worldMatrix = calculateLocalMatrix();

		m_isDirty = false;
	}

	Matrix4x4 Transform::calculateLocalMatrix() const
	{
		// translation
		Matrix4x4 translation = Translation(m_position);
		
		// rotation
		Matrix4x4 rotX = Rotation(AngleAxis(m_rotation.x, Vector3(1, 0, 0)));
		Matrix4x4 rotY = Rotation(AngleAxis(m_rotation.y, Vector3(0, 1, 0)));
		Matrix4x4 rotZ = Rotation(AngleAxis(m_rotation.z, Vector3(0, 0, 1)));
		Matrix4x4 rotation = rotY * rotX * rotZ;	// Note: Yaw-Pitch-Roll order (Y * X * Z)

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
		m_rotation = rotation;
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
		}
	}
}