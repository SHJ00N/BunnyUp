#include "pch.h"
#include "Collider_Legacy.h"
#include "Scene.h"
#include "DebugRenderer.h"
#include "GameObject.h"
#include "ImGuiClass.h"

#include <algorithm>

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
		const Vector3 extents = GetWorldExtents();
		const auto& rotation = ownerGameObject->transform.GetRotationMatrix();

		Matrix3x3 absR;
		for (int r = 0; r < 3; r++)
		{
			for (int c = 0; c < 3; c++)
			{
				absR.m[r][c] = std::abs(rotation.m[r][c]);
			}
		}

		Vector3 aabbExtents = extents * absR;

		return AABB(GetWorldCenter(), aabbExtents.x, aabbExtents.y, aabbExtents.z);
	}

	Matrix3x3 BoxCollider::GetInverseInertia(float mass) const
	{
		float w = extents.x * 2.0f;
		float h = extents.y * 2.0f;
		float d = extents.z * 2.0f;
		float ww = w * w;
		float hh = h * h;
		float dd = d * d;
		float MassDiv12 = 1.0f / 12.0f * mass;

		Vector3 inertiaLocal = { MassDiv12 * (hh + dd), MassDiv12 * (ww + dd), MassDiv12 * (ww + hh) };

		Matrix3x3 result{ };
		result.m[0][0] = 1.0f / inertiaLocal.x;
		result.m[1][1] = 1.0f / inertiaLocal.y;
		result.m[2][2] = 1.0f / inertiaLocal.z;

		return result;
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
		const float maxScale = std::max(scale.x, std::max(scale.y, scale.z));

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

	Matrix3x3 SphereCollider::GetInverseInertia(float mass) const
	{
		float i = (2.0f / 5.0f) * mass * radius * radius;

		Matrix3x3 result{ };
		result.m[0][0] = 1.0f / i;
		result.m[1][1] = 1.0f / i;
		result.m[2][2] = 1.0f / i;

		return result;
	}

	void SphereCollider::OnImGui()
	{
		ImGui::DragFloat3("Center", &center.x, 0.01f);
		ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, 10000.0f);
		
		ImGui::Checkbox("Trigger", &this->m_isTrigger);
	}
}