#include "pch.h"
#include "Physics_Legacy.h"
#include "Scene.h"
#include "Collision_Legacy.h"
#include "BoundingVolume.h"
#include "Log.h"
#include "Rigidbody_Legacy.h"

#include <algorithm>

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

        updateRigidbody(scene, fdt);
        
        // Current frame collision detection
        updateCollider(scene);
        broadPhase(scene);
        narrowPhase();

        // rigidbody process
        buildContactConstraints(fdt);
        // apply previous accumulated impulse
        for (auto& constraint : m_constraints)
        {
            if (constraint.needWarm)
            {
                warmStart(constraint);
            }
        }
        solveConstraints();
        updateContactImpulse(); // update contact's accumulated impulse for caching
        integrateTransform(scene, fdt);
        
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
        m_prevContacts.swap(m_contacts);
        m_contacts.clear();

        // Narrow phase - precise collision checks for candidate pairs
        for (const CollisionPair& pair : m_candidateCollisionPairs)
        {
            Contact contact;
            for (int i = 0; i < 4; i++)
            {
                contact.accumulatedNormalImpulse[i] = 0.0f;
                if (i < 2)
                {
                    contact.accumulatedTangentImpulse[i] = 0.0f;
                }
            }
            if (Collision::Intersects(pair.a, pair.b, contact))
            {
                m_currentCollisionPairs.insert(pair);

                // find previous contact
                Contact* oldContact = nullptr;
                for (auto& prev : m_prevContacts)
                {
                    if (prev.pair == contact.pair)
                    {
                        oldContact = &prev;
                        break;
                    }
                }

                if (oldContact)
                {
                    int matched = 0;
                    for (unsigned int i = 0; i < contact.pointCount; i++)
                    {
                        for (unsigned int j = 0; j < oldContact->pointCount; j++)
                        {
                            if (contact.ids[i] != oldContact->ids[j]) continue;
                            if (LengthSq(contact.contactPoints[i] - oldContact->contactPoints[j]) > 1e-2f) continue;
                            
                            contact.accumulatedNormalImpulse[i] = oldContact->accumulatedNormalImpulse[j];
                            matched++;
                            break;
                        }
                    }

                    float ratio = (float)matched / (float)contact.pointCount;
                    if (ratio > 0.5f)
                    {
                        contact.accumulatedTangentImpulse[0] = oldContact->accumulatedTangentImpulse[0];
                        contact.accumulatedTangentImpulse[1] = oldContact->accumulatedTangentImpulse[1];
                    }
                }

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

    void Physics::updateRigidbody(Scene* scene, float fdt)
    {
        const auto& colliders = scene->GetColliders();
        for (auto collider : colliders)
        {
            auto rigidbody = collider->ownerGameObject->GetComponent<Rigidbody>();

            if (!rigidbody) continue;
            if (rigidbody->type != RigidbodyType::Dynamic) continue;

            // apply gravity
            if (rigidbody->useGravity)
            {
                constexpr Vector3 gravity(0.0f, -9.81f, 0.0f);
                rigidbody->linearVelocity += gravity * fdt;
            }
        }
    }

    void Physics::updateContactImpulse()
    {
        for (const auto& constraint : m_constraints)
        {
            int index = constraint.contactIndex;
            for (int i = 0; i < 4; i++)
            {
                m_contacts[index].accumulatedNormalImpulse[i] = constraint.accumulatedNormalImpulse[i];
            }
            for (int i = 0; i < 2; i++)
            {
                m_contacts[index].accumulatedTangentImpulse[i] = constraint.accumulatedTangentImpulse[i];
            }
        }
    }

    void Physics::calculatePlaneSpace(const Vector3& n, Vector3& p, Vector3& q)
    {
        if (std::fabs(n.z) > 0.70710678f)
        {
            // choose p in y-z plane
            float a = n.y * n.y + n.z * n.z;
            float k = 1.f / sqrt(a);
            p.x = 0;
            p.y = -n.z * k;
            p.z = n.y * k;
            // set q = n x p
            q.x = a * k;
            q.y = -n.x * p.z;
            q.z = n.x * p.y;
        }
        else
        {
            // choose p in x-y plane
            float a = n.x * n.x + n.y * n.y;
            float k = 1.f / sqrt(a);
            p.x = -n.y * k;
            p.y = n.x * k;
            p.z = 0;
            // set q = n x p
            q.x = -n.z * p.y;
            q.y = n.z * p.x;
            q.z = a * k;
        }
    }

    void Physics::buildContactConstraints(float fdt)
    {
        m_constraints.clear();

        float invFdt = 1.0f / fdt;

        constexpr float positionDrift = 0.005f;
        constexpr float positionConstraintCoeff = 0.05f;

        int curContactIndex = 0;
        for (const auto& contact : m_contacts)
        {
            if (contact.pointCount == 0)
            {
                curContactIndex++;
                continue;
            }

            ContactConstraint constraint;
            for (int i = 0; i < 4; i++)
            {
                constraint.points[i] = 0.0f;
                constraint.normalMass[i] = 0.0f;
                constraint.accumulatedNormalImpulse[i] = 0.0f;
            }
            constraint.tangentMass[0] = constraint.tangentMass[1] = 0.0f;
            constraint.accumulatedTangentImpulse[0] = constraint.accumulatedTangentImpulse[1] = 0.0f;

            constraint.contactIndex = curContactIndex++;

            auto* rigidbodyA = contact.pair.a->ownerGameObject->GetComponent<Rigidbody>();
            auto* rigidbodyB = contact.pair.b->ownerGameObject->GetComponent<Rigidbody>();
            if (!rigidbodyA && !rigidbodyB) continue;

            constraint.bodyA = rigidbodyA;
            constraint.bodyB = rigidbodyB;

            constraint.positionA = contact.pair.a->GetWorldCenter();
            constraint.positionB = contact.pair.b->GetWorldCenter();

            constraint.normal = contact.normal;

            // rigid data
            Vector3 linVelA = rigidbodyA ? rigidbodyA->linearVelocity : Vector3(0.0f);
            Vector3 linVelB = rigidbodyB ? rigidbodyB->linearVelocity : Vector3(0.0f);

            Vector3 angVelA = rigidbodyA ? rigidbodyA->angularVelocity : Vector3(0.0f);
            Vector3 angVelB = rigidbodyB ? rigidbodyB->angularVelocity : Vector3(0.0f);

            float invMassA = rigidbodyA ? rigidbodyA->GetInverseMass() : 0.0f;
            float invMassB = rigidbodyB ? rigidbodyB->GetInverseMass() : 0.0f;

            Matrix3x3 invInertiaA = rigidbodyA ? rigidbodyA->GetWorldInverseInertia() : Matrix3x3(0.0f);
            Matrix3x3 invInertiaB = rigidbodyB ? rigidbodyB->GetWorldInverseInertia() : Matrix3x3(0.0f);

            // normal constraints
            for (uint32_t i = 0; i < contact.pointCount; ++i)
            {
                Vector3 rA = contact.contactPoints[i] - contact.pair.a->GetWorldCenter();
                Vector3 rB = contact.contactPoints[i] - contact.pair.b->GetWorldCenter();
                constraint.rA[i] = rA;
                constraint.rB[i] = rB;

                Vector3 angularA = Cross(rA, contact.normal);
                Vector3 angularB = Cross(rB, contact.normal) * -1.0f;

                constraint.normalMass[i] = -1.0f / (invMassA + invMassB + Dot(angularA * invInertiaA, angularA) + Dot(angularB * invInertiaB, angularB));
                float relVelN = Dot(contact.normal, linVelA) + Dot(contact.normal * -1.0f, linVelB) + Dot(angularA, angVelA) + Dot(angularB, angVelB);

                float restitution = std::max((rigidbodyA && rigidbodyA->type == RigidbodyType::Dynamic ? rigidbodyA->restitution : 0.0f), (rigidbodyB && rigidbodyB->type == RigidbodyType::Dynamic ? rigidbodyB->restitution : 0.0f));
                if (relVelN * relVelN < 0.004f) restitution = 0.0f;


                constraint.bias[i] = restitution * relVelN;
                if (contact.penetrations[i] > 0.001f)
                {
                    constraint.needWarm = true;
                }

                float penetration = std::max(0.0f, contact.penetrations[i] - positionDrift);
                constraint.bias[i] += penetration * positionConstraintCoeff;
                constraint.accumulatedNormalImpulse[i] = contact.accumulatedNormalImpulse[i];
            }

            // friction center
            Vector3 center = Vector3(0.0f);
            for (uint32_t i = 0; i < contact.pointCount; ++i)
            {
                center += contact.contactPoints[i];
            }
            center /= (float)contact.pointCount;
            constraint.center = center;
            constraint.friction = std::min((rigidbodyA ? rigidbodyA->friction : 1.0f), (rigidbodyB ? rigidbodyB->friction : 1.0f));

            // tangent constraints
            Vector3 tangent[2];
            calculatePlaneSpace(contact.normal, tangent[0], tangent[1]);

            Vector3 r[2];
            r[0] = center - contact.pair.a->GetWorldCenter();
            r[1] = center - contact.pair.b->GetWorldCenter();
            constraint.fr[0] = r[0];
            constraint.fr[1] = r[1];

            for (uint32_t i = 0; i < 2; ++i)
            {
                Vector3 angularA = Cross(r[0], tangent[i]);
                Vector3 angularB = Cross(r[1], tangent[i]) * -1.0f;

                constraint.tangentMass[i] = -1.0f / (invMassA + invMassB + Dot(angularA * invInertiaA, angularA) + Dot(angularB * invInertiaB, angularB));
                constraint.accumulatedTangentImpulse[i] = contact.accumulatedTangentImpulse[i];
            }

            // contact points
            constraint.pointCount = contact.pointCount;
            for (uint32_t i = 0; i < contact.pointCount; ++i)
            {
                constraint.points[i] = contact.contactPoints[i];
            }

            m_constraints.push_back(constraint);
        }
    }

    void Physics::warmStart(ContactConstraint& constraint)
    {
        auto* bodyA = constraint.bodyA;
        auto* bodyB = constraint.bodyB;

        float invMassA = bodyA ? bodyA->GetInverseMass() : 0.0f;
        float invMassB = bodyB ? bodyB->GetInverseMass() : 0.0f;

        Matrix3x3 invInertiaA = bodyA ? bodyA->GetWorldInverseInertia() : Matrix3x3(0.0f);
        Matrix3x3 invInertiaB = bodyB ? bodyB->GetWorldInverseInertia() : Matrix3x3(0.0f);

        Vector3 normal = constraint.normal * -1.0f;
        
        // apply normal impulse
        for (uint32_t i = 0; i < constraint.pointCount; i++)
        {
            Vector3 linear = normal * -1.0f;
            float lambda = constraint.accumulatedNormalImpulse[i];

            Vector3 angularA = Cross(constraint.rA[i], normal) * -1.0f;
            Vector3 angularB = Cross(constraint.rB[i], normal);

            // apply impulse
            Vector3 linImpA = invMassA * linear * lambda;
            Vector3 linImpB = invMassB * (linear * -1.0f) * lambda;
            Vector3 angImpA = (angularA * invInertiaA) * lambda;
            Vector3 angImpB = (angularB * invInertiaB) * lambda;

            if (bodyA)
            {
                bodyA->linearVelocity += linImpA;
                bodyA->angularVelocity += angImpA;
            }
            if (bodyB)
            {
                bodyB->linearVelocity += linImpB;
                bodyB->angularVelocity += angImpB;
            }
        }

        // apply tangent impulse
        Vector3 tangent[2];
        calculatePlaneSpace(normal, tangent[0], tangent[1]);

        for (int i = 0; i < 2; i++)
        {
            Vector3 linear = tangent[i] * -1.0f;
            Vector3 angularA = Cross(constraint.fr[0], tangent[i]) * -1.0f;
            Vector3 angularB = Cross(constraint.fr[1], tangent[i]);

            float lambda = constraint.accumulatedTangentImpulse[i];

            // apply impulse
            Vector3 linImpA = invMassA * linear * lambda;
            Vector3 linImpB = invMassB * (linear * -1.0f) * lambda;
            Vector3 angImpA = (angularA * invInertiaA) * lambda;
            Vector3 angImpB = (angularB * invInertiaB) * lambda;

            if (bodyA)
            {
                bodyA->linearVelocity += linImpA;
                bodyA->angularVelocity += angImpA;
            }
            if (bodyB)
            {
                bodyB->linearVelocity += linImpB;
                bodyB->angularVelocity += angImpB;
            }
        }
    }

    void Physics::solveConstraints()
    {
        int m_nIterations = 8;
        for (int i = 0; i < m_nIterations; ++i)
        {
            for (auto& constraint : m_constraints)
            {
                float maxRambdaDt[4] = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
                float minRambdaDt[4] = { 0.f, 0.f, 0.f, 0.f };
                solveImpulse(constraint, maxRambdaDt, minRambdaDt);
            }
        }

        for (int i = 0; i < m_nIterations; ++i)
        {
            for (auto& constraint : m_constraints)
            {
                float maxRambdaDt[4] = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
                float minRambdaDt[4] = { 0.f, 0.f, 0.f, 0.f };

                float sum = 0;
                for (int j = 0; j < 4; j++)
                {
                    sum += constraint.accumulatedNormalImpulse[j];
                }
                for (int j = 0; j < 4; j++)
                {
                    maxRambdaDt[j] = constraint.friction * sum;
                    minRambdaDt[j] = -maxRambdaDt[j];
                }

                solveFriction(constraint, maxRambdaDt, minRambdaDt);
            }
        }
    }

    void Physics::solveImpulse(ContactConstraint& constraint, float maxRambdaDt[4], float minRambdaDt[4])
    {
        auto* bodyA = constraint.bodyA;
        auto* bodyB = constraint.bodyB;

        Vector3 linVelA = bodyA ? bodyA->linearVelocity : Vector3(0.0f);
        Vector3 linVelB = bodyB ? bodyB->linearVelocity : Vector3(0.0f);

        Vector3 angVelA = bodyA ? bodyA->angularVelocity : Vector3(0.0f);
        Vector3 angVelB = bodyB ? bodyB->angularVelocity : Vector3(0.0f);

        float invMassA = bodyA ? bodyA->GetInverseMass() : 0.0f;
        float invMassB = bodyB ? bodyB->GetInverseMass() : 0.0f;

        Matrix3x3 invInertiaA = bodyA ? bodyA->GetWorldInverseInertia() : Matrix3x3(0.0f);
        Matrix3x3 invInertiaB = bodyB ? bodyB->GetWorldInverseInertia() : Matrix3x3(0.0f);

        Vector3 normal = constraint.normal * -1.0f;

        Vector3 linear = normal * -1.0f;
        for (uint32_t i = 0; i < constraint.pointCount; i++)
        {
            Vector3 angularA = Cross(constraint.rA[i], normal) * -1.0f;
            Vector3 angularB = Cross(constraint.rB[i], normal);
            float rambdaDt = Dot(linear, linVelA) + Dot(linear * -1.0f, linVelB) + Dot(angularA, angVelA) + Dot(angularB, angVelB);
            rambdaDt += constraint.bias[i];
            rambdaDt *= constraint.normalMass[i];

            // accumulated impulse
            float prevSum = constraint.accumulatedNormalImpulse[i];
            float updated = prevSum;
            updated += rambdaDt;
            updated = std::max(updated, minRambdaDt[i]);
            updated = std::min(updated, maxRambdaDt[i]);
            rambdaDt = updated - prevSum;
            constraint.accumulatedNormalImpulse[i] = updated;

            // apply impulse
            Vector3 linImpA = invMassA * linear * rambdaDt;
            Vector3 linImpB = invMassB * (linear * -1.0f) * rambdaDt;
            Vector3 angImpA = (angularA * invInertiaA) * rambdaDt;
            Vector3 angImpB = (angularB * invInertiaB) * rambdaDt;

            if (bodyA)
            {
                bodyA->linearVelocity += linImpA;
                bodyA->angularVelocity += angImpA;
            }
            if (bodyB)
            {
                bodyB->linearVelocity += linImpB;
                bodyB->angularVelocity += angImpB;
            }
        }
    }

    void Physics::solveFriction(ContactConstraint& constraint, float maxRambdaDt[4], float minRambdaDt[4])
    {
        if (constraint.tangentMass[0] == 0 && constraint.tangentMass[1] == 0) return;

        auto* bodyA = constraint.bodyA;
        auto* bodyB = constraint.bodyB;

        Vector3 n = constraint.normal * -1.0f;

        // tangent constraints
        Vector3 tangent[2];
        calculatePlaneSpace(n, tangent[0], tangent[1]);

        float invMassA = bodyA ? bodyA->GetInverseMass() : 0.0f;
        float invMassB = bodyB ? bodyB->GetInverseMass() : 0.0f;

        Matrix3x3 invInertiaA = bodyA ? bodyA->GetWorldInverseInertia() : Matrix3x3(0.0f);
        Matrix3x3 invInertiaB = bodyB ? bodyB->GetWorldInverseInertia() : Matrix3x3(0.0f);

        // accumulated impulse
        for (int i = 0; i < 2; i++)
        {
            Vector3 linVelA = bodyA ? bodyA->linearVelocity : Vector3(0.0f);
            Vector3 linVelB = bodyB ? bodyB->linearVelocity : Vector3(0.0f);

            Vector3 angVelA = bodyA ? bodyA->angularVelocity : Vector3(0.0f);
            Vector3 angVelB = bodyB ? bodyB->angularVelocity : Vector3(0.0f);

            Vector3 linear = tangent[i] * -1.0f;
            Vector3 angularA = Cross(constraint.fr[0], tangent[i]) * -1.0f;
            Vector3 angularB = Cross(constraint.fr[1], tangent[i]);
            float rambdaDt = Dot(linear, linVelA) + Dot(linear * -1.0f, linVelB) + Dot(angularA, angVelA) + Dot(angularB, angVelB);
            rambdaDt *= constraint.tangentMass[i];

            // accumulated impulse
            float prevSum = constraint.accumulatedTangentImpulse[i];
            float updated = prevSum;
            updated += rambdaDt;
            updated = std::max(updated, minRambdaDt[i]);
            updated = std::min(updated, maxRambdaDt[i]);
            rambdaDt = updated - prevSum;
            constraint.accumulatedTangentImpulse[i] = updated;

            // apply impulse
            Vector3 linImpA = invMassA * linear * rambdaDt;
            Vector3 linImpB = invMassB * (linear * -1.0f) * rambdaDt;
            Vector3 angImpA = (angularA * invInertiaA) * rambdaDt;
            Vector3 angImpB = (angularB * invInertiaB) * rambdaDt;

            if (bodyA)
            {
                bodyA->linearVelocity += linImpA;
                bodyA->angularVelocity += angImpA;
            }
            if (bodyB)
            {
                bodyB->linearVelocity += linImpB;
                bodyB->angularVelocity += angImpB;
            }
        }

        //	angular damping for point constraint
        Vector3 ab = Normalize((constraint.positionB - constraint.positionA));
        Vector3 ac = Normalize((constraint.center - constraint.positionA));
        if (Dot(ab, ac) > 0.95f || (invMassA == 0.f || invMassB == 0.f))
        {
            if (bodyA)
            {
                float angNA = Dot(n, bodyA->angularVelocity);
                bodyA->angularVelocity -= (angNA * 0.1f) * n;
            }
            if (bodyB)
            {
                float angNB = Dot(n, bodyB->angularVelocity);
                bodyB->angularVelocity -= (angNB * 0.1f) * n;
            }
        }
    }

    void Physics::integrateTransform(Scene* scene, float fdt)
    {
        const float BT_GPU_ANGULAR_MOTION_THRESHOLD = (0.25f * 3.14159254f);
        const float angularDamping = 0.05f;
        
        const auto& colliders = scene->GetColliders();
        for (auto collider : colliders)
        {
            auto rigidbody = collider->ownerGameObject->GetComponent<Rigidbody>();
            
            if (!rigidbody) continue;
            if (rigidbody->type != RigidbodyType::Dynamic) continue;

            // linear velocity
            if (LengthSq(rigidbody->linearVelocity) < 0.025f)
            {
                rigidbody->linearVelocity = Vector3(0.0f);
            }
            rigidbody->position += rigidbody->linearVelocity * fdt;

            // angular velocity
            Vector3 axis;
            rigidbody->angularVelocity *= std::pow(1.0f - angularDamping, fdt);
            
            Vector3 angVel = rigidbody->angularVelocity;
            float fAngle = std::sqrt(Dot(angVel, angVel));
            // limit the angular motion
            if (fAngle * fdt > BT_GPU_ANGULAR_MOTION_THRESHOLD)
            {
                fAngle = BT_GPU_ANGULAR_MOTION_THRESHOLD / fdt;
            }
            if (fAngle < 0.001f)
            {
                // use Taylor's expansions of sync function
                axis = angVel * (0.5f * fdt - (fdt * fdt * fdt) * 0.020833333333f * fAngle * fAngle);
            }
            else
            {
                // sync(fAngle) = sin(c*fAngle)/t
                axis = angVel * (std::sin(0.5f * fAngle * fdt) / fAngle);
            }

            Quaternion dorn;
            dorn.x = axis.x;
            dorn.y = axis.y;
            dorn.z = axis.z;
            dorn.w = std::cos(fAngle * fdt * 0.5f);
            Quaternion orn0 = rigidbody->orientation;

            Quaternion predictedOrn = dorn * orn0;
            predictedOrn = Normalize(predictedOrn);
            rigidbody->orientation = predictedOrn;

            Transform& transform = rigidbody->ownerGameObject->transform;
            transform.SetLocalPosition(rigidbody->position);
            transform.SetLocalRotation(rigidbody->orientation);
        }
    }
}