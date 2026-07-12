#pragma once

#include "MathHelper.h"

namespace Engine
{
	class Rp3dCollider;
	struct CollisionData
	{
		Rp3dCollider* other = nullptr;
		Vector3 normal;
		float penetrationDepth = 0.0f;

		CollisionData() : other(nullptr) {}
		CollisionData(Rp3dCollider* other, const Vector3& normal, float penetrationDepth) : other(other), normal(normal), penetrationDepth(penetrationDepth) {}
	};
}