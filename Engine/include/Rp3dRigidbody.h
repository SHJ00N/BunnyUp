// license ---------------------------------------------------------------------------------
// This class is wrapper for ReactPhysics3d
// ReactPhysics3d : https://github.com/DanielChappuis/reactphysics3d
// -------------------------------------------------------------------------------------------

#pragma once

#include "Component.h"
#include "MathHelper.h"

#include <reactphysics3d/reactphysics3d.h>

namespace Engine
{
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
		const uint64_t GetId() const { return id; }
		BodyType GetType() const { return m_type; }
		rp3d::RigidBody* GetRigidBody() const { return m_body; }

		// physics utility
		void AddForce(const Vector3& force);
		void AddForce(const Vector3& force, const Vector3& pos);
		void AddImpulse(const Vector3& force);

	protected:
		void OnAwake() override;	// initialized rigidbody
		void OnDestroy() override;		// destroy rp3d rigidbody from rp3d physics world

	private:
		rp3d::PhysicsWorld* m_world;		// rp3d rigidbody owner pointer

		rp3d::RigidBody* m_body;	// rp3d rigidbody pointer
		BodyType m_type;
		float m_mass;
		uint64_t id;

		static uint64_t nextId;
	};
}