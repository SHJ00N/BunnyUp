#pragma once

#include "MathHelper.h"

namespace Engine
{
	class Collider;
	class Rigidbody;

	struct CollisionPair
	{
		Collider* a = nullptr;
		Collider* b = nullptr;

		CollisionPair() = default;

		CollisionPair(Collider* colliderA, Collider* colliderB)
		{
			// Ensure consistent ordering (a < b) to avoid duplicate pairs
			if (colliderA->GetColliderID() < colliderB->GetColliderID())
			{
				a = colliderA;
				b = colliderB;
			}
			else
			{
				a = colliderB;
				b = colliderA;
			}
		}

		// internal equality operator used collider id  for CollisionPair, used for unordered_set
		bool operator==(const CollisionPair& other) const
		{
			return a->GetColliderID() == other.a->GetColliderID() && b->GetColliderID() == other.b->GetColliderID();
		}
	};

	// Custom hash function for CollisionPair to be used in unordered_set
	struct CollisionPairHash
	{
		size_t operator()(const CollisionPair& pair) const
		{
			size_t h1 = std::hash<uint64_t>()(pair.a->GetColliderID());
			size_t h2 = std::hash<uint64_t>()(pair.b->GetColliderID());

			return h1 ^ (h2 << 1);
		}
	};

	constexpr int MaxContacts = 4;
	struct Contact
	{
		CollisionPair pair;

		Vector3 contactPoints[MaxContacts];
		float penetrations[MaxContacts];
		uint32_t pointCount = 0;

		Vector3 normal;

		uint32_t ids[MaxContacts];

		float accumulatedNormalImpulse[4];
		float accumulatedTangentImpulse[2];

		// internal equality operator used collider id  for CollisionPair, used for unordered_set
		bool operator==(const Contact& other) const
		{
			return pair == other.pair;
		}
	};

	struct ContactConstraint
	{
		int contactIndex;
		bool needWarm = false;
		Rigidbody* bodyA;
		Rigidbody* bodyB;

		Vector3 positionA;
		Vector3 positionB;

		Vector3 normal;	// contact normal
		// contact points
		Vector3 points[4];
		Vector3 rA[4];
		Vector3 rB[4];
		uint32_t pointCount;
		float friction;
		float normalMass[4];
		float bias[4];
		float accumulatedNormalImpulse[4];

		Vector3 center; // for friction
		Vector3 fr[2];
		float tangentMass[2];	// for friction
		float accumulatedTangentImpulse[2];	// forfriction
	};
}