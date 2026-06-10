#pragma once

#include "Collider.h"
#include "Contact.h"

namespace Engine
{
	class Collision
	{
	public:
		static bool Intersects(Collider* colliderA, Collider* colliderB, Contact& contact);
		static bool IntersectsBoxToBox(BoxCollider* boxA, BoxCollider* boxB, Contact& contact);
		static bool IntersectsBoxToSphere(BoxCollider* box, SphereCollider* sphere, Contact& contact);
		static bool IntersectsSphereToSphere(SphereCollider* sphereA, SphereCollider* sphereB, Contact& contact);
	};
}