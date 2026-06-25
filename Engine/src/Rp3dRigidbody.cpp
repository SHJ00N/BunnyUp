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
		: m_world(nullptr), m_body(nullptr), m_mass(mass), m_type(type), useGravity(useGravity)
	{
		id = nextId++;
	}

	Rp3dRigidbody::~Rp3dRigidbody()
	{
	}

	void Rp3dRigidbody::OnAwake()
	{
		m_world = ownerGameObject->scene->GetPhysicsSystem()->GetPhysicsWorld();

		const auto position = ownerGameObject->transform.GetWorldPosition();
		const auto rotation = ownerGameObject->transform.GetWorldRotationQuaternion();

		// create rigidbody
		const rp3d::Transform rp3dTransform(rp3d::Vector3(position.x, position.y, position.z), rp3d::Quaternion(rotation.x, rotation.y, rotation.z, rotation.w));
		m_body = m_world->createRigidBody(rp3dTransform);
		// configure rigidbody
		m_body->setMass(m_mass);
		m_body->setType(static_cast<rp3d::BodyType>(m_type));
		m_body->enableGravity(useGravity);
		m_body->setIsDebugEnabled(true);

		EventBus::GetInstance().Publish< RigidbodyCreateEvent>(RigidbodyCreateEvent{ this });
	}

	void Rp3dRigidbody::OnDestroy()
	{
		if (!m_world || !m_body) return;

		m_world->destroyRigidBody(m_body);
		m_body = nullptr;

		EventBus::GetInstance().Publish< RigidbodyDestroyEvent>(RigidbodyDestroyEvent{ this });
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

	void Rp3dRigidbody::AddForce(const Vector3& force)
	{
		m_body->applyLocalForceAtCenterOfMass(rp3d::Vector3(force.x, force.y, force.z));
	}

	void Rp3dRigidbody::AddForce(const Vector3& force, const Vector3& pos)
	{
		m_body->applyLocalForceAtLocalPosition(rp3d::Vector3(force.x, force.y, force.z), rp3d::Vector3(pos.x, pos.y, pos.z));
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

		ImGui::DragFloat("Mass", &m_mass, 0.1f, 1.0f, 50.0f);

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
