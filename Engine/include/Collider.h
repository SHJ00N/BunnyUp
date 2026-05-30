#pragma once

#include "Component.h"
#include "MathHelper.h"

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
		Vector3 center = { 0.0f, 0.0f, 0.0f };

		// virtual functions
		virtual void BuildDebugRender(DebugRenderer* renderer) const = 0;
		Vector3 GetWorldCenter() const;

		ColliderType GetColliderType() const { return m_type; }
		// set up trigger
		bool isTrigger() const { return m_isTrigger; }
		void SetTrigger(bool value) { m_isTrigger = value; }

	protected:
		Collider(ColliderType type) : m_type(type) { }
		virtual ~Collider() = default;

		// collider regist to scene
		void OnAwake() override;
		void OnDestroy() override;
	private:
		ColliderType m_type = ColliderType::None;
		bool m_isTrigger = false;
	};

	class BoxCollider : public Collider
	{
	public:
		BoxCollider() : Collider(ColliderType::OBB) { }
		Vector3 extents = { 0.5f, 0.5f, 0.5f };

		// override
		void BuildDebugRender(DebugRenderer* renderer) const override;
		void OnImGui() override;
	};

	class SphereCollider : public Collider
	{
	public:
		SphereCollider() : Collider(ColliderType::Sphere) { }
		float radius = 0.0f;

		// override
		void BuildDebugRender(DebugRenderer* renderer) const override;
		void OnImGui() override;
	};
}