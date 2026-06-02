#include "Collider.h"
#include "Scene.h"
#include "DebugRenderer.h"
#include "GameObject.h"
#include "ImGuiClass.h"

namespace Engine
{
	// ---------------------------------------------------------------------
	// Collider
	// ---------------------------------------------------------------------
	Collider::Collider(ColliderType type) : m_type(type), m_colliderID(s_nextID++) 
	{
	}

	void Collider::OnAwake()
	{
		ownerGameObject->scene->RegisterCollider(this);
	}

	void Collider::OnDestroy()
	{
		ownerGameObject->scene->UnregisterCollider(this);
	}

	Vector3 Collider::GetWorldCenter() const
	{
		const auto& world = ownerGameObject->transform.GetWorldMatrix();
		Vector4 worldPos = Vector4(center.x, center.y, center.z, 1.0f) * world;
		return { worldPos.x, worldPos.y, worldPos.z };
	}
	
	// ---------------------------------------------------------------------
	// Box Collider
	// ---------------------------------------------------------------------
	Vector3 BoxCollider::GetWorldAABBExtents() const
	{
		const auto& world = ownerGameObject->transform.GetWorldMatrix();
		// world axis
		const Vector3 right(world.m00, world.m01, world.m02);
		const Vector3 up(world.m10, world.m11, world.m12);
		const Vector3 forward(world.m20, world.m21, world.m22);

		// local -> world
		const float ex = std::abs(right.x) * extents.x + std::abs(up.x) * extents.y + std::abs(forward.x) * extents.z;
		const float ey = std::abs(right.y) * extents.x + std::abs(up.y) * extents.y + std::abs(forward.y) * extents.z;
		const float ez = std::abs(right.z) * extents.x + std::abs(up.z) * extents.y + std::abs(forward.z) * extents.z;

		return Vector3(ex, ey, ez);
	}

	Vector3 BoxCollider::GetWorldExtents() const
	{
		const Vector3 scale = ownerGameObject->transform.GetLossyScale();

		return { extents.x * std::abs(scale.x), extents.y * std::abs(scale.y), extents.z * std::abs(scale.z) };
	}

	AABB BoxCollider::GetBounds() const
	{
		const auto& worldExtents = GetWorldExtents();

		return AABB(GetWorldCenter(), worldExtents.x, worldExtents.y, worldExtents.z);
	}

	void BoxCollider::BuildDebugRender(DebugRenderer* renderer) const
	{
		renderer->AddBox(center, extents, ownerGameObject->transform, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	void BoxCollider::OnImGui()
	{
		ImGui::DragFloat3("Center", &center.x, 0.01f);
		ImGui::DragFloat3("Extents", &extents.x, 0.1f, 0.0f, 10000.0f);

		ImGui::Checkbox("Trigger", &this->m_isTrigger);
	}

	// ---------------------------------------------------------------------
	// Sphere Collider
	// ---------------------------------------------------------------------
	float SphereCollider::GetWorldRadius() const
	{
		const Vector3 scale = ownerGameObject->transform.GetLocalScale();
		const float maxScale = max(scale.x, max(scale.y, scale.z));

		return radius * maxScale;
	}

	AABB SphereCollider::GetBounds() const
	{
		const float worldRadius = GetWorldRadius();

		return AABB(GetWorldCenter(), worldRadius, worldRadius, worldRadius);
	}

	void SphereCollider::BuildDebugRender(DebugRenderer* renderer) const
	{
		renderer->AddSphere(this->GetWorldCenter(), radius, ownerGameObject->transform, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	void SphereCollider::OnImGui()
	{
		ImGui::DragFloat3("Center", &center.x, 0.01f);
		ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, 10000.0f);
		
		ImGui::Checkbox("Trigger", &this->m_isTrigger);
	}
}