#include "pch.h"
#include "Raycast.h"
#include "GameObject.h"
#include "Rp3dCollider.h"

namespace Engine
{
	RaycastCallback::RaycastCallback(RaycastHit& hit, float maxDistance)
		: m_hit(hit), m_maxDistance(maxDistance)
	{
	}

	rp3d::decimal RaycastCallback::notifyRaycastHit(const rp3d::RaycastInfo& info)
	{
		auto* collider = static_cast<Rp3dCollider*>(info.collider->getUserData());

		m_hit.collider = collider;
		m_hit.hitObject = collider ? collider->ownerGameObject : nullptr;

		m_hit.point = Vector3(
			static_cast<float>(info.worldPoint.x),
			static_cast<float>(info.worldPoint.y),
			static_cast<float>(info.worldPoint.z));

		m_hit.normal = Vector3(
			static_cast<float>(info.worldNormal.x),
			static_cast<float>(info.worldNormal.y),
			static_cast<float>(info.worldNormal.z));

		m_hit.distance = static_cast<float>(info.hitFraction) * m_maxDistance;

		// Return the hit fraction to clip the raycast to this point
		return info.hitFraction;
	}

}