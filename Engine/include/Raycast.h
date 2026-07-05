#pragma once

#include <reactphysics3d/reactphysics3d.h>
#include "MathHelper.h"

namespace Engine
{
	class GameObject;
	class Rp3dCollider;

	struct Ray
	{
		Vector3 start;
		Vector3 end;
		float distance = 0.0f;
	};

	struct RaycastHit
	{
		Rp3dCollider* collider = nullptr;
		GameObject* hitObject = nullptr;

		Vector3 point;
		Vector3 normal;
		float distance = 0.0f;
	};

	class RaycastCallback : public rp3d::RaycastCallback
	{
	public:
		explicit RaycastCallback(RaycastHit& hit, float maxDistance);
		rp3d::decimal notifyRaycastHit(const rp3d::RaycastInfo& info) override;

	private:
		RaycastHit& m_hit;
		float m_maxDistance;
	};
}