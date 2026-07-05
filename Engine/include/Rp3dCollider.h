// license ---------------------------------------------------------------------------------
// This class is wrapper for ReactPhysics3d
// ReactPhysics3d : https://github.com/DanielChappuis/reactphysics3d
// -------------------------------------------------------------------------------------------

#pragma once

#include "Component.h"
#include "MathHelper.h"
#include "CollisionMask.h"

#include <reactphysics3d/reactphysics3d.h>

namespace Engine
{
	class Rp3dRigidbody;
	class Rp3dCollider : public Component
	{
	public:
		Rp3dCollider();
		Rp3dCollider(const Vector3& position, const Quaternion& rotation, float friction, float bounciness, bool trigger, CollisionLayer layer = CollisionLayer::Default);
		~Rp3dCollider() = default;

		// override
		void OnImGui() override;

		// setter
		void SetLocalPosition(const Vector3& position);
		void SetLocalRotation(const Quaternion& rotation);
		void SetFriction(float friction);
		void SetBounciness(float bounciness);
		void SetTrigger(bool value);
		void SetCollisionLayer(CollisionLayer layer);
		// getter
		bool IsTrigger() const { return m_isTrigger; }
		float GetFriction() const { return m_friction; }
		float GetBounciness() const { return m_bounciness; }
		CollisionLayer GetCollisionLayer() const { return m_layer; }
		Vector3 GetLocalPosition() const { return m_center; }

		void UpdateCollisionMask();

	protected:
		rp3d::Collider* m_collider = nullptr;
		Rp3dRigidbody* m_rigidbody = nullptr;
		// collider local transform
		Vector3 m_center = Vector3(0.0f);
		Quaternion m_rotation = Quaternion::Identity();

		// rp3d collider material
		float m_friction = 1.0f;
		float m_bounciness = 0.01f;
		
		bool m_isTrigger = false;

		// override
		void OnDestroy() override;

	private:
		CollisionLayer m_layer = CollisionLayer::Default;
	};

	class Rp3dBoxCollider : public Rp3dCollider
	{
	public:
		Rp3dBoxCollider();
		Rp3dBoxCollider(const Vector3& size);
		Rp3dBoxCollider(const Vector3& pos, const Quaternion& rot, const Vector3& size, float friction, float bounce, bool trigger, CollisionLayer layer = CollisionLayer::Default);
		// override
		void OnImGui() override;

		void SetSize(const Vector3& size);
		Vector3 GetSize() const { return m_size; }

	protected:
		void OnStart() override;
		void OnDestroy() override;

	private:
		Vector3 m_size = Vector3(1.0f);
		rp3d::BoxShape* m_shape = nullptr;
	};

	class Rp3dSphereCollider : public Rp3dCollider
	{
	public:
		Rp3dSphereCollider();
		Rp3dSphereCollider(float radius);
		Rp3dSphereCollider(const Vector3& pos, const Quaternion& rot, float radius, float friction, float bounce, bool trigger, CollisionLayer layer = CollisionLayer::Default);

		// override
		void OnImGui() override;

		void SetRadius(float radius);
		float GetRadius(float radius) const { return m_radius; }

	protected:
		void OnStart() override;
		void OnDestroy() override;

	private:
		float m_radius = 0.5f;
		rp3d::SphereShape* m_shape = nullptr;
	};

	class Rp3dCapsuleCollider : public Rp3dCollider
	{
	public:
		Rp3dCapsuleCollider();
		Rp3dCapsuleCollider(float radius, float height);
		Rp3dCapsuleCollider(const Vector3& pos, const Quaternion& rot, float radius, float height, float friction, float bounce, bool trigger, CollisionLayer layer = CollisionLayer::Default);
		// override
		void OnImGui() override;

		void SetRadius(float radius);
		void SetHeight(float height);

		float GetRadius() const { return m_radius; }
		float GetHeight() const { return m_height; }

	protected:
		void OnStart() override;
		void OnDestroy() override;

	private:
		float m_radius = 0.5f;
		float m_height = 2.0f;
		rp3d::CapsuleShape* m_shape = nullptr;
	};
}