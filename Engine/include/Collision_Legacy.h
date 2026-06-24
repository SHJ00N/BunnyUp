#pragma once

#include "Collider_Legacy.h"
#include "Contact_Legacy.h"

namespace Engine
{
	enum class OBBContactType
	{
		Face,
		Edge
	};

	enum class AxisType
	{
		Right,
		Up,
		Forward
	};

	struct OBBData
	{
		Vector3 center;
		Vector3 extents;

		Vector3 axis[3];

		uint64_t id;
	};

	struct ManifoldPoint
	{
		Vector3 point;

		uint32_t id;
	};

	class Collision
	{
	public:
		static bool Intersects(Collider* colliderA, Collider* colliderB, Contact& contact);
		static bool IntersectsBoxToBox(BoxCollider* boxA, BoxCollider* boxB, Contact& contact);
		static bool IntersectsBoxToSphere(BoxCollider* box, SphereCollider* sphere, Contact& contact);
		static bool IntersectsSphereToSphere(SphereCollider* sphereA, SphereCollider* sphereB, Contact& contact);
	private:
		// OBB contact utility
		static OBBData BuildOBBData(BoxCollider* box);
		static std::vector<ManifoldPoint> BuildFaceManifold(const OBBData& obbA, const OBBData& obbB, const Vector3& collisionNormal);
		static Vector3 BuildEdgeContact(const OBBData& obbA, const OBBData& obbB, int axisAIndex, int axisBIndex, const Vector3& collisionNormal);
	};
}