#include "pch.h"
#include "Rp3dCollider.h"
#include "Rp3dRigidbody.h"
#include "GameObject.h"
#include "Scene.h"
#include "PhysicsSystem.h"
#include "Log.h"
#include "ImGuiClass.h"

namespace Engine
{
	// --------------------------------------------------------------------------------
	//									Base Collider
	// --------------------------------------------------------------------------------
	Rp3dCollider::Rp3dCollider()
	{
	}

	Rp3dCollider::Rp3dCollider(const Vector3& position, const Quaternion& rotation, float friction, float bounciness, bool trigger)
		: m_center(position), m_rotation(rotation), m_friction(friction), m_bounciness(bounciness), m_isTrigger(trigger)
	{
	}

	void Rp3dCollider::OnDestroy()
	{
		if (m_rigidbody && m_rigidbody->GetRigidBody() && m_collider)
		{
			m_rigidbody->GetRigidBody()->removeCollider(m_collider);
			m_collider = nullptr;
		}
	}

	void Rp3dCollider::SetLocalPosition(const Vector3& position)
	{
		m_center = position;

		if (!m_collider) return;

		rp3d::Transform rp3dTransform(rp3d::Vector3(m_center.x, m_center.y, m_center.z), rp3d::Quaternion(m_rotation.x, m_rotation.y, m_rotation.z, m_rotation.w));
		m_collider->setLocalToBodyTransform(rp3dTransform);
	}

	void Rp3dCollider::SetLocalRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;

		if (!m_collider) return;

		rp3d::Transform rp3dTransform(rp3d::Vector3(m_center.x, m_center.y, m_center.z), rp3d::Quaternion(m_rotation.x, m_rotation.y, m_rotation.z, m_rotation.w));
		m_collider->setLocalToBodyTransform(rp3dTransform);
	}

	void Rp3dCollider::SetFriction(float friction)
	{
		m_friction = friction;

		if (!m_collider) return;

		auto& material = m_collider->getMaterial();
		material.setFrictionCoefficient(friction);
	}

	void Rp3dCollider::SetBounciness(float bounciness)
	{
		m_bounciness = bounciness;

		if (!m_collider) return;

		auto& material = m_collider->getMaterial();
		material.setBounciness(bounciness);
	}

	void Rp3dCollider::SetTrigger(bool value)
	{
		m_isTrigger = value;

		if (!m_collider) return;

		m_collider->setIsTrigger(value);
	}

	void Rp3dCollider::OnImGui()
	{
		if (ImGui::DragFloat3("Center", &m_center.x, 0.01f))
		{
			SetLocalPosition(m_center);
		}

		ImGui::DragFloat3("Rotation", &m_rotation.x, 0.01f);

		if (ImGui::DragFloat("Friction", &m_friction, 0.01f, 0.0f, 1.0f))
		{
			SetFriction(m_friction);
		}

		if (ImGui::DragFloat("Bounciness", &m_bounciness, 0.01f, 0.0f, 1.0f))
		{
			SetBounciness(m_bounciness);
		}

		if (ImGui::Checkbox("Trigger", &m_isTrigger))
		{
			SetTrigger(m_isTrigger);
		}
	}

	// --------------------------------------------------------------------------------
	//									Box Collider
	// --------------------------------------------------------------------------------
	Rp3dBoxCollider::Rp3dBoxCollider()
	{
	}

	Rp3dBoxCollider::Rp3dBoxCollider(const Vector3& size)
		: m_size(size)
	{
	}

	Rp3dBoxCollider::Rp3dBoxCollider(const Vector3& pos, const Quaternion& rot, const Vector3& size, float friction, float bounce, bool trigger)
		: Rp3dCollider(pos, rot, friction, bounce, trigger), m_size(size)
	{
	}

	void Rp3dBoxCollider::OnStart()
	{
		auto& common = ownerGameObject->scene->GetPhysicsSystem()->GetPhysicsCommon();
		auto* body = ownerGameObject->GetComponent<Rp3dRigidbody>();
		if (!body || !body->GetRigidBody())
		{
			return;
		}
		m_rigidbody = body;

		m_shape = common.createBoxShape(rp3d::Vector3(m_size.x * 0.5f, m_size.y * 0.5f, m_size.z * 0.5f));
		
		rp3d::Transform rp3dTransform(rp3d::Vector3(m_center.x, m_center.y, m_center.z), rp3d::Quaternion(m_rotation.x, m_rotation.y, m_rotation.z, m_rotation.w));
		m_collider = body->GetRigidBody()->addCollider(m_shape, rp3dTransform);

		auto& material = m_collider->getMaterial();
		material.setFrictionCoefficient(m_friction);
		material.setBounciness(m_bounciness);

		m_collider->setIsTrigger(m_isTrigger);
	}

	void Rp3dBoxCollider::OnDestroy()
	{
		Rp3dCollider::OnDestroy();

		if (m_shape)
		{
			auto& common = ownerGameObject->scene->GetPhysicsSystem()->GetPhysicsCommon();
			common.destroyBoxShape(m_shape);
			m_shape = nullptr;
		}
	}

	void Rp3dBoxCollider::SetSize(const Vector3& size)
	{
		m_size = size;

		if (m_shape)
		{
			m_shape->setHalfExtents(rp3d::Vector3(m_size.x * 0.5f, m_size.y * 0.5f, m_size.z * 0.5f));
		}
	}

	void Rp3dBoxCollider::OnImGui()
	{
		Rp3dCollider::OnImGui();

		if (ImGui::DragFloat3("Size", &m_size.x, 0.01f, 0.2f, 10000.0f))
		{
			SetSize(m_size);
		}
	}

	// --------------------------------------------------------------------------------
	//									Sphere Collider
	// --------------------------------------------------------------------------------
	Rp3dSphereCollider::Rp3dSphereCollider()
	{
	}

	Rp3dSphereCollider::Rp3dSphereCollider(float radius)
		: m_radius(radius)
	{
	}

	Rp3dSphereCollider::Rp3dSphereCollider(const Vector3& pos, const Quaternion& rot, float radius, float friction, float bounce, bool trigger)
		: Rp3dCollider(pos, rot, friction, bounce, trigger), m_radius(radius)
	{
	}

	void Rp3dSphereCollider::OnStart()
	{
		auto& common = ownerGameObject->scene->GetPhysicsSystem()->GetPhysicsCommon();
		auto* body = ownerGameObject->GetComponent<Rp3dRigidbody>();
		if (!body || !body->GetRigidBody()) return;

		m_rigidbody = body;

		m_shape = common.createSphereShape(m_radius);

		rp3d::Transform rp3dTransform(rp3d::Vector3(m_center.x, m_center.y, m_center.z), rp3d::Quaternion(m_rotation.x, m_rotation.y, m_rotation.z, m_rotation.w));
		m_collider = body->GetRigidBody()->addCollider(m_shape, rp3dTransform);

		auto& material = m_collider->getMaterial();
		material.setFrictionCoefficient(m_friction);
		material.setBounciness(m_bounciness);

		m_collider->setIsTrigger(m_isTrigger);
	}

	void Rp3dSphereCollider::OnDestroy()
	{
		Rp3dCollider::OnDestroy();

		if (m_shape)
		{
			auto& common = ownerGameObject->scene->GetPhysicsSystem()->GetPhysicsCommon();
			common.destroySphereShape(m_shape);
			m_shape = nullptr;
		}
	}

	void Rp3dSphereCollider::SetRadius(float radius)
	{
		m_radius = radius;

		if (m_shape)
		{
			m_shape->setRadius(radius);
		}
	}

	void Rp3dSphereCollider::OnImGui()
	{
		Rp3dCollider::OnImGui();

		if (ImGui::DragFloat("Radius", &m_radius, 0.1f, 0.1f, 10000.0f))
		{
			SetRadius(m_radius);
		}
	}

	// --------------------------------------------------------------------------------
	//									Capsule Collider
	// --------------------------------------------------------------------------------
	Rp3dCapsuleCollider::Rp3dCapsuleCollider()
	{
	}

	Rp3dCapsuleCollider::Rp3dCapsuleCollider(float radius, float height)
		: m_radius(radius), m_height(height)
	{
	}

	Rp3dCapsuleCollider::Rp3dCapsuleCollider(const Vector3& pos, const Quaternion& rot, float radius, float height, float friction, float bounce, bool trigger)
		: Rp3dCollider(pos, rot, friction, bounce, trigger), m_radius(radius), m_height(height)
	{
	}

	void Rp3dCapsuleCollider::OnStart()
	{
		auto& common = ownerGameObject->scene->GetPhysicsSystem()->GetPhysicsCommon();
		auto* body = ownerGameObject->GetComponent<Rp3dRigidbody>();
		if (!body || !body->GetRigidBody()) return;

		m_rigidbody = body;

		m_shape = common.createCapsuleShape(m_radius, m_height);

		rp3d::Transform rp3dTransform(rp3d::Vector3(m_center.x, m_center.y, m_center.z), rp3d::Quaternion(m_rotation.x, m_rotation.y, m_rotation.z, m_rotation.w));
		m_collider = body->GetRigidBody()->addCollider(m_shape, rp3dTransform);

		auto& material = m_collider->getMaterial();
		material.setFrictionCoefficient(m_friction);
		material.setBounciness(m_bounciness);

		m_collider->setIsTrigger(m_isTrigger);
	}

	void Rp3dCapsuleCollider::OnDestroy()
	{
		Rp3dCollider::OnDestroy();

		if (m_shape)
		{
			auto& common = ownerGameObject->scene->GetPhysicsSystem()->GetPhysicsCommon();
			common.destroyCapsuleShape(m_shape);
			m_shape = nullptr;
		}
	}

	void Rp3dCapsuleCollider::SetRadius(float radius)
	{
		m_radius = radius;

		if (m_shape)
		{
			m_shape->setRadius(radius);
		}
	}

	void Rp3dCapsuleCollider::SetHeight(float height)
	{
		m_height = height;

		if (m_shape)
		{
			m_shape->setHeight(height);
		}
	}

	void Rp3dCapsuleCollider::OnImGui()
	{
		Rp3dCollider::OnImGui();

		if (ImGui::DragFloat("Radius", &m_radius, 0.1f, 0.1f, 10000.0f))
		{
			SetRadius(m_radius);
		}

		if (ImGui::DragFloat("Height", &m_height, 0.1f, 0.1f, 10000.0f))
		{
			SetHeight(m_height);
		}
	}
}