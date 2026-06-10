#pragma once

#include "Collider.h"
#include "BVHNode.h"
#include "EventBus.h"
#include "Contact.h"

#include <unordered_set>
#include <vector>
#include <memory>

namespace Engine
{
	using namespace BVH;
	struct CollisionPair
	{
		Collider* a;
		Collider* b;

		CollisionPair() = default;

		CollisionPair(Collider* colliderA, Collider* colliderB)
		{
			// Ensure consistent ordering (a < b) to avoid duplicate pairs
			if (colliderA->GetColliderID() <colliderB->GetColliderID())
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

	class Scene;
	class Physics
	{
	public:
		void Initialize(Scene* scene);
		void Update(Scene* scene, float fdt);
		void Shutdown();

		BVHNode* GetBVHRoot() const { return m_bvhRoot.get(); }
		std::unique_ptr<BVHNode>& GetBVHRootPtr() { return m_bvhRoot; }

		bool IsInitialized() const { return m_isInitialized; }
		void RemoveCollisionPair(Collider* collider);
	private:
		bool m_isInitialized = false;
		std::unordered_set<CollisionPair, CollisionPairHash> m_currentCollisionPairs;
		std::unordered_set<CollisionPair, CollisionPairHash> m_previousCollisionPairs;

		std::unique_ptr<BVHNode> m_bvhRoot;
		std::vector<CollisionPair> m_candidateCollisionPairs;

		std::vector<Contact> m_contacts;	// contacted pairs
		// bvh state member
		float m_buildAreaRatio = 0.0f;
		float m_buildArea = 0.0f;
		// There are various policies for rebuilding the tree. Here, we use the area ratio and root bounds growth.
		bool needRebuildBVH(int colliderCount);
		// collision detection phases
		void updateCollider(Scene* scene);
		void broadPhase(Scene* scene);
		void collectPairs(BVHNode* a, BVHNode* b);	// helper function for broad phase to collect potential collision pairs from BVH traversal
		void narrowPhase();
		// collision event processing
		void processCollisionEvents();

		// rigidbody process
		void updateRigidbody(Scene* scene, float fdt);
		void processCollisionReactions();
		void resolvePenetration(const Contact& contact);
		void resolveImpulse(const Contact& contact);

		ListenerID m_objectDestroyedListenerID;
	};
}