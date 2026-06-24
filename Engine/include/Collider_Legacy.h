#pragma once

#include "Component.h"
#include "MathHelper.h"
#include "BoundingVolume.h"
#include "BVHNode.h"

namespace Engine
{
	class DebugRenderer;

	enum class ColliderType
	{
		None,
		Sphere,
		OBB
	};

	class Collider : public Component
	{
	public:
		BVH::BVHNode* bvhNode = nullptr;	// pointer to the BVH node that contains this collider, set by physics system when building BVH
		Vector3 center = { 0.0f, 0.0f, 0.0f };
		Vector3 GetWorldCenter() const;
		uint64_t GetColliderID() const { return m_colliderID; }

		// virtual functions
		virtual void BuildDebugRender(DebugRenderer* renderer) const = 0;
		virtual AABB GetBounds() const = 0;
		virtual Matrix3x3 GetInverseInertia(float mass) const = 0;

		ColliderType GetColliderType() const { return m_type; }
		// set up trigger
		bool IsTrigger() const { return m_isTrigger; }
		void SetTrigger(bool value) { m_isTrigger = value; }

	protected:
		Collider(ColliderType type);
		virtual ~Collider() = default;

		bool m_isTrigger = false;

		// collider regist to scene
		void OnAwake() override;
		void OnDestroy() override;
	private:
		ColliderType m_type = ColliderType::None;
		uint64_t m_colliderID = 0;	// unique ID for collider, used for collision detection and management

		inline static uint64_t s_nextID = 1;	// for generating unique IDs for colliders
	};

	class BoxCollider : public Collider
	{
	public:
		BoxCollider() : Collider(ColliderType::OBB) { }
		Vector3 extents = { 0.5f, 0.5f, 0.5f };

		// override
		AABB GetBounds() const override;
		void BuildDebugRender(DebugRenderer* renderer) const override;
		void OnImGui() override;
		Matrix3x3 GetInverseInertia(float mass) const override;

		// utility
		Vector3 GetWorldAABBExtents() const;
		Vector3 GetWorldExtents() const;
	};

	class SphereCollider : public Collider
	{
	public:
		SphereCollider() : Collider(ColliderType::Sphere) { }
		float radius = 1.0f;

		// override
		AABB GetBounds() const override;
		void BuildDebugRender(DebugRenderer* renderer) const override;
		void OnImGui() override;
		Matrix3x3 GetInverseInertia(float mass) const override;

		// utility
		float GetWorldRadius() const;
	};
}