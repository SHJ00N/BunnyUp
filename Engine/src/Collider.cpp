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
		const Transform& transform = ownerGameObject->transform;
		return transform.GetWorldPosition() + transform.GetRight() * center.x + transform.GetUp() * center.y + transform.GetForward() * center.z;
	}
	
	// ---------------------------------------------------------------------
	// Box Collider
	// ---------------------------------------------------------------------
	void BoxCollider::BuildDebugRender(DebugRenderer* renderer) const
	{
		renderer->AddBox(center, extents, ownerGameObject->transform, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	void BoxCollider::OnImGui()
	{
		ImGui::DragFloat3("Center", &center.x, 0.01f);
		ImGui::DragFloat3("Extents", &extents.x, 0.01f, 0.0f, 100.0f);
	}

	// ---------------------------------------------------------------------
	// Sphere Collider
	// ---------------------------------------------------------------------
	void SphereCollider::BuildDebugRender(DebugRenderer* renderer) const
	{
		renderer->AddSphere(this->GetWorldCenter(), radius, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	void SphereCollider::OnImGui()
	{
		ImGui::DragFloat3("Center", &center.x, 0.01f);
		ImGui::DragFloat("Radius", &radius, 0.01f, 0.0f, 100.0f);
	}
}