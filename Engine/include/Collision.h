#pragma once

#include "Collider.h"

namespace Engine
{
	class Collision
	{
	public:
		static bool Intersects(Collider* colliderA, Collider* colliderB);
		static bool IntersectsBoxToBox(BoxCollider* boxA, BoxCollider* boxB);
		static bool IntersectsBoxToSphere(BoxCollider* box, SphereCollider* sphere);
		static bool IntersectsSphereToSphere(SphereCollider* sphereA, SphereCollider* sphereB);
	};
}