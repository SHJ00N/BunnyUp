#include "pch.h"
#include "Rp3dCollider.h"
#include "Rp3dRigidbody.h"
#include "GameObject.h"
#include "Scene.h"
#include "PhysicsSystem.h"
#include "Log.h"

namespace Engine
{
	// --------------------------------------------------------------------------------
	//									Base Collider
	// --------------------------------------------------------------------------------
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

	}

	// --------------------------------------------------------------------------------
	//									Box Collider
	// --------------------------------------------------------------------------------
	void Rp3dBoxCollider::OnStart()
	{
		auto& common = ownerGameObject->scene->GetPhysicsSystem()->GetPhysicsCommon();
		auto* body = ownerGameObject->GetComponent<Rp3dRigidbody>();
		if (!body || !body->GetRigidBody()) return;
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

	}

	// --------------------------------------------------------------------------------
	//									Sphere Collider
	// --------------------------------------------------------------------------------
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

	}

	// --------------------------------------------------------------------------------
	//									Capsule Collider
	// --------------------------------------------------------------------------------
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

	}
}