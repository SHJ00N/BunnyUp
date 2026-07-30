#include "pch.h"
#include "Rp3dRigidbody.h"
#include "GameObject.h"
#include "Scene.h"
#include "PhysicsSystem.h"
#include "ImGuiClass.h"
#include "Log.h"

namespace Engine
{
	uint64_t Rp3dRigidbody::nextId = 0;

	Rp3dRigidbody::Rp3dRigidbody(float mass, BodyType type, bool useGravity)
		: m_physics(nullptr), m_body(nullptr), m_mass(mass), m_type(type), useGravity(useGravity)
	{
		id = nextId++;
	}

	Rp3dRigidbody::~Rp3dRigidbody()
	{
	}

	void Rp3dRigidbody::OnAwake()
	{
		m_physics = ownerGameObject->scene->GetPhysicsSystem();
		assert(m_physics && "Physcis system is null");

		auto* world = m_physics->GetPhysicsWorld();

		const auto position = ownerGameObject->transform.GetWorldPosition();
		const auto rotation = ownerGameObject->transform.GetWorldRotationQuaternion();

		// create rigidbody
		const rp3d::Transform rp3dTransform(rp3d::Vector3(position.x, position.y, position.z), rp3d::Quaternion(rotation.x, rotation.y, rotation.z, rotation.w));
		m_body = world->createRigidBody(rp3dTransform);
		// configure rigidbody
		m_body->setMass(m_mass);
		m_body->setType(static_cast<rp3d::BodyType>(m_type));
		m_body->enableGravity(useGravity);
		m_body->setIsDebugEnabled(true);
		m_body->setAngularLockAxisFactor(rp3d::Vector3((float)m_angleLockX, (float)m_angleLockY, (float)m_angleLockZ));

		m_body->setUserData(this);

		m_physics->AddRigidbody(this);
	}

	void Rp3dRigidbody::OnEnable()
	{
		if (!m_body) return;
		m_body->setIsActive(true);
	}

	void Rp3dRigidbody::OnDisable()
	{
		if (!m_body) return;
		m_body->setIsActive(false);
	}

	void Rp3dRigidbody::OnDestroy()
	{
		auto* world = m_physics->GetPhysicsWorld();
		if (!world || !m_body) return;

		world->destroyRigidBody(m_body);
		m_body = nullptr;

		m_physics->RemoveRigidbody(this);
	}


	void Rp3dRigidbody::AddCollider(Rp3dCollider* collider)
	{
		m_colliders.push_back(collider);
	}

	void Rp3dRigidbody::RemoveCollider(Rp3dCollider* collider)
	{
		m_colliders.erase(std::remove(m_colliders.begin(), m_colliders.end(), collider), m_colliders.end());
	}

	void Rp3dRigidbody::SyncTransformToPhysics()
	{
		const auto position = ownerGameObject->transform.GetWorldPosition();
		const auto rotation = ownerGameObject->transform.GetWorldRotationQuaternion();

		const rp3d::Transform rp3dTransform(rp3d::Vector3(position.x, position.y, position.z), rp3d::Quaternion(rotation.x, rotation.y, rotation.z, rotation.w));
		m_body->setTransform(rp3dTransform);
	}

	void Rp3dRigidbody::SyncPhysicsToTransform()
	{
		const auto& rp3dTransform = m_body->getTransform();
		const auto& rp3dPosition = rp3dTransform.getPosition();
		const auto& rp3dRotation = rp3dTransform.getOrientation();

		ownerGameObject->transform.SetLocalPosition(Vector3(rp3dPosition.x, rp3dPosition.y, rp3dPosition.z));
		ownerGameObject->transform.SetLocalRotation(Quaternion(rp3dRotation.x, rp3dRotation.y, rp3dRotation.z, rp3dRotation.w));
	}

	Vector3 Rp3dRigidbody::GetLinearVelocity() const
	{
		const auto& rp3dLinear = m_body->getLinearVelocity();
		return Vector3(rp3dLinear.x, rp3dLinear.y, rp3dLinear.z);
	}

	Vector3 Rp3dRigidbody::GetAngularVelocity() const
	{
		const auto& rp3dAngular = m_body->getAngularVelocity();
		return Vector3(rp3dAngular.x, rp3dAngular.y, rp3dAngular.z);
	}

	void Rp3dRigidbody::SetLinearVelocity(const Vector3& velocity)
	{
		m_body->setLinearVelocity(rp3d::Vector3(velocity.x, velocity.y, velocity.z));
	}

	void Rp3dRigidbody::SetLinearDamping(float value)
	{
		m_body->setLinearDamping(value);
	}

	void Rp3dRigidbody::SetAngularVelocity(const Vector3& velocity)
	{
		m_body->setAngularVelocity(rp3d::Vector3(velocity.x, velocity.y, velocity.z));
	}

	void Rp3dRigidbody::SetAngularLock(bool x, bool y, bool z)
	{
		m_angleLockX = x;
		m_angleLockY = y;
		m_angleLockZ = z;
	}

	void Rp3dRigidbody::SetGravity(bool value)
	{
		useGravity = value;
		m_body->enableGravity(value);
	}

	void Rp3dRigidbody::SetBodyType(BodyType type)
	{
		m_type = type;
		m_body->setType(static_cast<rp3d::BodyType>(type));
	}

	void Rp3dRigidbody::SetMass(float mass)
	{
		m_mass = mass;
	}

	void Rp3dRigidbody::SetActive(bool value)
	{
		m_body->setIsActive(value);
	}

	void Rp3dRigidbody::AddForce(const Vector3& force)
	{
		m_body->applyLocalForceAtCenterOfMass(rp3d::Vector3(force.x, force.y, force.z));
	}

	void Rp3dRigidbody::AddForce(const Vector3& force, const Vector3& pos)
	{
		m_body->applyLocalForceAtLocalPosition(rp3d::Vector3(force.x, force.y, force.z), rp3d::Vector3(pos.x, pos.y, pos.z));
	}

	void Rp3dRigidbody::AddImpulse(const Vector3& impulse)
	{
		auto linear = m_body->getLinearVelocity();
		linear += rp3d::Vector3(impulse.x, impulse.y, impulse.z) / m_body->getMass();

		m_body->setLinearVelocity(linear);
	}

	void Rp3dRigidbody::OnImGui()
	{
		const char* rigidbodyTypes[] =
		{
			"Static",
			"Kinematic",
			"Dynamic"
		};

		int currentType = static_cast<int>(m_type);

		if (ImGui::Combo("Type", &currentType, rigidbodyTypes, IM_ARRAYSIZE(rigidbodyTypes)))
		{
			SetBodyType(static_cast<BodyType>(currentType));
		}

		ImGui::DragFloat("Mass", &m_mass, 0.1f, 1.0f, 1000.0f);

		if (ImGui::Checkbox("Gravity", &useGravity))
		{
			SetGravity(useGravity);
		}

		const auto& rp3dLinear = m_body->getLinearVelocity();
		Vector3 liner = { rp3dLinear.x, rp3dLinear.y, rp3dLinear.z };
		ImGui::DragFloat3("Linear", &liner.x, 0.01f);

		const auto& rp3dAngular = m_body->getAngularVelocity();
		Vector3 angular = { rp3dAngular.x, rp3dAngular.y, rp3dAngular.z };
		ImGui::DragFloat3("Angular", &angular.x, 0.01f);
	}
}
