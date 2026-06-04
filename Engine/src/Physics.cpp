#include "Physics.h"
#include "Scene.h"
#include "Collision.h"
#include "BoundingVolume.h"
#include "Log.h"

namespace Engine
{
    void Physics::Initialize(Scene* scene)
    {
        m_currentCollisionPairs.clear();
        m_previousCollisionPairs.clear();
		
        // Build BVH for the initial set of colliders in the scene
        const auto& colliders = scene->GetColliders();
        std::vector<Collider*> colliderPtrs(colliders.begin(), colliders.end());
        m_bvhRoot = BVH::BuildBVH(colliderPtrs, 0, static_cast<int>(colliderPtrs.size()));
        if (m_bvhRoot)
        {
            m_buildArea = m_bvhRoot->bounds.SurfaceArea();
            m_buildAreaRatio = ComputeAreaRatio(m_bvhRoot.get());
        }
        
        // subscribe object destroying event
        m_objectDestroyedListenerID = EventBus::GetInstance().Subscribe<ObjectDestroyedEvent>(
            [this](const ObjectDestroyedEvent& e)
            {
                auto collider = e.object->GetComponent<Collider>();
                if (collider)
                {
                    RemoveCollisionPair(collider);
                }
            }
        );

        m_isInitialized = true;
	}

	void Physics::Update(Scene* scene, float fdt)
	{
		// Swap current and previous collision pairs for the next frame
        m_previousCollisionPairs.swap(m_currentCollisionPairs);
        m_currentCollisionPairs.clear();

		// Current frame collision detection
        updateCollider(scene);
		broadPhase(scene);
        narrowPhase();
		// dispatch collision events based on current and previous collision pairs
        processCollisionEvents();
	}

    void Physics::Shutdown()
    {
        EventBus::GetInstance().Unsubscribe<ObjectDestroyedEvent>(m_objectDestroyedListenerID);
    }

    void Physics::collectPairs(BVHNode* a, BVHNode* b)
    {
        if (!a || !b)
        {
            return;
        }

        // Broad phase pruning
        if (!a->bounds.IsIntersectsAABB(b->bounds))
        {
            return;
        }

        // Same node
        if (a == b)
        {
            if (a->IsLeaf())
            {
                return;
            }

            collectPairs(a->left.get(), a->left.get());
            collectPairs(a->right.get(), a->right.get());
            collectPairs(a->left.get(), a->right.get());

            return;
        }

        // Both leaf nodes
        if (a->IsLeaf() && b->IsLeaf())
        {
            m_candidateCollisionPairs.emplace_back(a->collider, b->collider);
            return;
        }

        // One leaf, one internal
        if (a->IsLeaf())
        {
            collectPairs(a, b->left.get());
            collectPairs(a, b->right.get());
            return;
        }

        if (b->IsLeaf())
        {
            collectPairs(a->left.get(), b);
            collectPairs(a->right.get(), b);
            return;
        }

        // Both internal
        collectPairs(a->left.get(), b);
        collectPairs(a->right.get(), b);
    }

    bool Physics::needRebuildBVH(int colliderCount)
    {
        if (!m_bvhRoot)
        {
            return false;
        }
        // get ratio of current size to build size
        float currentAreaRatio = ComputeAreaRatio(m_bvhRoot.get());
        float ratio = currentAreaRatio / m_buildAreaRatio;

        float growth = m_bvhRoot->bounds.SurfaceArea() / m_buildArea;

        return ratio > 1.3f || growth > 2.0f;
    }

    void Physics::updateCollider(Scene* scene)
    {
        const auto& colliders = scene->GetColliders();
        for (auto collider : colliders)
        {
            BVHNode* leaf = collider->bvhNode;

            if (leaf == nullptr)
            {
                continue;
            }
            if (leaf->bounds.Contains(collider->GetBounds()))
            {
                continue;
            }
            // collider is out from bvh bounds
            // The tree is reconstructed in the order of remove/insert -> rotation -> rebuild. Tree reconstruction can be terminated at each stage.
            auto leafPtr = RemoveLeaf(m_bvhRoot, leaf);
            leafPtr->bounds = CreateFatAABB(collider->GetBounds());
            InsertLeaf(m_bvhRoot, std::move(leafPtr));

            if (needRebuildBVH(static_cast<int>(colliders.size())))
            {
                LOG_INFO("BVH Rebuild");
                std::vector<Collider*> colliderPtrs(colliders.begin(), colliders.end());
                m_bvhRoot = BuildBVH(colliderPtrs, 0, static_cast<int>(colliderPtrs.size()));
                if (m_bvhRoot)
                {
                    m_buildArea = m_bvhRoot->bounds.SurfaceArea();
                    m_buildAreaRatio = ComputeAreaRatio(m_bvhRoot.get());
                }
            }
        }
    }

	void Physics::broadPhase(Scene* scene)
	{
		m_candidateCollisionPairs.clear();
        const auto& colliders = scene->GetColliders();
		// Naive O(n^2) broad phase - can be optimized with spatial partitioning (e.g., quadtrees, octrees, BVH)
     //   for (size_t i = 0; i < colliders.size(); ++i)
     //   {
     //       for (size_t j = i + 1; j < colliders.size(); ++j)
     //       {
     //           auto* a = colliders[i];
     //           auto* b = colliders[j];

     //           if (Collision::Intersects(a, b))
     //           {
					//m_currentCollisionPairs.emplace(a, b);
     //           }
     //       }
     //   }

        // collision detection
		collectPairs(m_bvhRoot.get(), m_bvhRoot.get());
	}

    void Physics::narrowPhase()
    {
		// Narrow phase - precise collision checks for candidate pairs
        for (const CollisionPair& pair : m_candidateCollisionPairs)
        {
            if (Collision::Intersects(pair.a, pair.b))
            {
                m_currentCollisionPairs.insert(pair);
            }
        }
    }

    void Physics::processCollisionEvents()
    {
        // Enter / Stay
        for (const CollisionPair& pair : m_currentCollisionPairs)
        {
			// check if either collider is a trigger
            const bool isTrigger = pair.a->IsTrigger() || pair.b->IsTrigger();

            const bool existedLastFrame = m_previousCollisionPairs.contains(pair);
            if (isTrigger)
            {
                if (existedLastFrame)
                {
                    pair.a->ownerGameObject->OnTriggerStay(pair.b);
                    pair.b->ownerGameObject->OnTriggerStay(pair.a);
                }
                else
                {
                    pair.a->ownerGameObject->OnTriggerEnter(pair.b);
                    pair.b->ownerGameObject->OnTriggerEnter(pair.a);
                }
            }
            else
            {
                if (existedLastFrame)
                {
                    pair.a->ownerGameObject->OnCollisionStay(pair.b);
                    pair.b->ownerGameObject->OnCollisionStay(pair.a);
                }
                else
                {
                    pair.a->ownerGameObject->OnCollisionEnter(pair.b);
                    pair.b->ownerGameObject->OnCollisionEnter(pair.a);
                }
            }
        }

        // Exit
        for (const CollisionPair& pair : m_previousCollisionPairs)
        {
            if (m_currentCollisionPairs.contains(pair))
            {
                continue;
            }

            const bool isTrigger = pair.a->IsTrigger() || pair.b->IsTrigger();
            if (isTrigger)
            {
                pair.a->ownerGameObject->OnTriggerExit(pair.b);
                pair.b->ownerGameObject->OnTriggerExit(pair.a);
            }
            else
            {
                pair.a->ownerGameObject->OnCollisionExit(pair.b);
                pair.b->ownerGameObject->OnCollisionExit(pair.a);
            }
        }
    }

    void Physics::RemoveCollisionPair(Collider* collider)
    {
        std::erase_if(
            m_previousCollisionPairs,
            [collider](const CollisionPair& pair)
            {
                return pair.a == collider || pair.b == collider;
            });

        std::erase_if(
            m_currentCollisionPairs,
            [collider](const CollisionPair& pair)
            {
                return pair.a == collider || pair.b == collider;
            });
    }
}