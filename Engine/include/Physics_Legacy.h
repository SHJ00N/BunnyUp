#pragma once

#include "Collider_Legacy.h"
#include "BVHNode.h"
#include "EventBus.h"
#include "Contact_Legacy.h"

#include <unordered_set>
#include <vector>
#include <memory>

namespace Engine
{
	using namespace BVH;

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

		std::vector<Contact> m_prevContacts;
		std::vector<Contact> m_contacts;	// contacted pairs
		std::vector<ContactConstraint> m_constraints;
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
		void updateContactImpulse();
		void buildContactConstraints(float fdt);
		void calculatePlaneSpace(const Vector3& n, Vector3& p, Vector3& q);
		void warmStart(ContactConstraint& constraint);
		void integrateTransform(Scene* scene, float fdt);
		void solveConstraints();
		void solveImpulse(ContactConstraint& constraint, float maxRambdaDt[4], float minRambdaDt[4]);
		void solveFriction(ContactConstraint& constraint, float maxRambdaDt[4], float minRambdaDt[4]);

		ListenerID m_objectDestroyedListenerID;
	};
}