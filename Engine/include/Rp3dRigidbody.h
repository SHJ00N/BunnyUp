// license ---------------------------------------------------------------------------------
// This class is wrapper for ReactPhysics3d
// ReactPhysics3d : https://github.com/DanielChappuis/reactphysics3d
// -------------------------------------------------------------------------------------------

#pragma once

#include "Component.h"
#include "MathHelper.h"

#include <vector>
#include <reactphysics3d/reactphysics3d.h>

namespace Engine
{
	class PhysicsSystem;

	enum class BodyType
	{
		STATIC,
		KINEMATIC,
		DYNAMIC
	};
	
	class Rp3dRigidbody : public Component
	{
	public:
		bool useGravity;

		Rp3dRigidbody(float mass = 1.0f, BodyType type = BodyType::STATIC, bool useGravity = false);
		~Rp3dRigidbody();

		// component override
		//void FixedUpdate(float fdt) override;
		void OnImGui() override;

		// synchronizing game object transform and rp3d rigidBody transform
		void SyncTransformToPhysics();
		void SyncPhysicsToTransform();

		void SetGravity(bool value);
		void SetBodyType(BodyType type);
		void SetLinearVelocity(const Vector3& velocity);
		void SetAngularVelocity(const Vector3& velocity);
		void SetLinearDamping(float value);
		void SetAngularLock(bool x, bool y, bool z);
		void SetMass(float mass);
		void SetActive(bool value);

		const uint64_t GetId() const { return id; }
		BodyType GetType() const { return m_type; }
		rp3d::RigidBody* GetRigidBody() const { return m_body; }
		PhysicsSystem* GetPhysicsSystem() const { return m_physics; }
		std::vector<Rp3dCollider*> GetColliders() const { return m_colliders; }
		Vector3 GetLinearVelocity() const;
		Vector3 GetAngularVelocity() const;
		float GetMass() const { return m_mass; }

		// physics utility
		void AddForce(const Vector3& force);
		void AddForce(const Vector3& force, const Vector3& pos);
		void AddImpulse(const Vector3& impulse);

		void AddCollider(Rp3dCollider* collider);
		void RemoveCollider(Rp3dCollider* collider);

	protected:
		void OnAwake() override;	// initialized rigidbody
		void OnDestroy() override;		// destroy rp3d rigidbody from rp3d physics world

		void OnEnable() override;
		void OnDisable() override;

	private:
		PhysicsSystem* m_physics;		// physics system pointer

		rp3d::RigidBody* m_body;	// rp3d rigidbody pointer
		BodyType m_type;
		float m_mass;
		uint64_t id;

		bool m_angleLockX = false;
		bool m_angleLockY = false;
		bool m_angleLockZ = false;

		std::vector<Rp3dCollider*> m_colliders;

		static uint64_t nextId;
	};
}