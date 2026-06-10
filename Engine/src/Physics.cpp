#include "Physics.h"
#include "Scene.h"
#include "Collision.h"
#include "BoundingVolume.h"
#include "Log.h"
#include "Rigidbody.h"

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

        // update rigidbody object
        updateRigidbody(scene, fdt);

        // Current frame collision detection
        updateCollider(scene);
        broadPhase(scene);
        narrowPhase();

        // process rigidbody
        processCollisionReactions();
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
        m_contacts.clear();

        // Narrow phase - precise collision checks for candidate pairs
        for (const CollisionPair& pair : m_candidateCollisionPairs)
        {
            Contact contact;
            if (Collision::Intersects(pair.a, pair.b, contact))
            {
                m_currentCollisionPairs.insert(pair);
                m_contacts.push_back(contact);
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

    void Physics::processCollisionReactions()
    {
        // offset position
        for (const auto& contact : m_contacts)
        {
            resolvePenetration(contact);
        }
        // update velocity
        for (const auto& contact : m_contacts)
        {
            resolveImpulse(contact);
        }
    }

    void Physics::updateRigidbody(Scene* scene, float fdt)
    {
        const auto& colliders = scene->GetColliders();
        const Vector3 gravity(0.0f, -9.81f, 0.0f);
        for (auto collider : colliders)
        {
            auto rigidbody = collider->ownerGameObject->GetComponent<Rigidbody>();
            // skip if does not exists rigidbody
            if (!rigidbody || rigidbody->type != RigidbodyType::Dynamic) continue;

            // apply gravity
            if (rigidbody->useGravity)
            {
                rigidbody->AddForce(gravity * rigidbody->mass);
            }
            Vector3 acceleration = rigidbody->GetAccumulatedForce() * rigidbody->GetInverseMass();
            rigidbody->velocity += acceleration * fdt;

            auto& transform = rigidbody->ownerGameObject->transform;
            transform.SetLocalPosition(transform.GetLocalPosition() + rigidbody->velocity * fdt);

            rigidbody->ClearForces();
        }
    }

    void Physics::resolvePenetration(const Contact& contact)
    {
        auto* rigidbodyA = contact.a->ownerGameObject->GetComponent<Rigidbody>();
        auto* rigidbodyB = contact.b->ownerGameObject->GetComponent<Rigidbody>();
        // get inverse mass
        float invMassA = rigidbodyA ? rigidbodyA->GetInverseMass() : 0.0f;
        float invMassB = rigidbodyB ? rigidbodyB->GetInverseMass() : 0.0f;
        float totalInvMass = invMassA + invMassB;

        // no penetration
        if (totalInvMass <= 0.0f)
        {
            return;
        }
        // resolve penetration
        Vector3 correction = contact.normal * (contact.penetration / totalInvMass);
        if (invMassA > 0.0f)
        {
            const auto& position = contact.a->ownerGameObject->transform.GetLocalPosition();
            contact.a->ownerGameObject->transform.SetLocalPosition(position - correction * invMassA);
        }
        if (invMassB > 0.0f)
        {
            const auto& position = contact.b->ownerGameObject->transform.GetLocalPosition();
            contact.b->ownerGameObject->transform.SetLocalPosition(position + correction * invMassB);
        }
    }

    void Physics::resolveImpulse(const Contact& contact)
    {
        auto* rigidbodyA = contact.a->ownerGameObject->GetComponent<Rigidbody>();
        auto* rigidbodyB = contact.b->ownerGameObject->GetComponent<Rigidbody>();
        if (!rigidbodyA && !rigidbodyB) return;

        // get inverse mass
        float invMassA = rigidbodyA ? rigidbodyA->GetInverseMass() : 0.0f;
        float invMassB = rigidbodyB ? rigidbodyB->GetInverseMass() : 0.0f;
        float totalInvMass = invMassA + invMassB;
        if (totalInvMass <= 0.0f)
        {
            return;
        }

        // get velocity
        Vector3 velocityA = rigidbodyA ? rigidbodyA->velocity : Vector3(0.0f);
        Vector3 velocityB = rigidbodyB ? rigidbodyB->velocity : Vector3(0.0f);
        
        // calculate nomal velocity
        Vector3 relativeVelocity = velocityB - velocityA;
        float velAlongNormal = Dot(relativeVelocity, contact.normal);
        // skip if they are already separating
        if (velAlongNormal > 0.0f)
        {
            return;
        }

        // get impulse vector
        float restitution = ((rigidbodyA ? rigidbodyA->restitution : 0.0f) + (rigidbodyB ? rigidbodyB->restitution : 0.0f)) * 0.5f;
        float j = -(1.0f + restitution) * velAlongNormal;
        j /= totalInvMass;
        Vector3 impulse = contact.normal * j;

        // apply impulse to each of velocity
        if (rigidbodyA)
        {
            rigidbodyA->velocity -= impulse * invMassA;
        }
        if (rigidbodyB)
        {
            rigidbodyB->velocity += impulse * invMassB;
        }
    }
}