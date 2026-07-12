#include "pch.h"
#include "PhysicsEventListener.h"
#include "EventBus.h"
#include "Rp3dCollider.h"
#include "Rp3dCollisionPair.h"
#include "Log.h"

namespace Engine
{
	void PhysicsEventListener::onContact(const rp3d::CollisionCallback::CallbackData& callbackData)
	{
        // For each contact pair
        for (uint32_t p = 0; p < callbackData.getNbContactPairs(); p++) {
            // Get the contact pair
            CollisionCallback::ContactPair contactPair = callbackData.getContactPair(p);
            
            auto* colliderA = static_cast<Rp3dCollider*>(contactPair.getCollider1()->getUserData());
            auto* colliderB = static_cast<Rp3dCollider*>(contactPair.getCollider2()->getUserData());

            Rp3dCollisionPair pair(colliderA, colliderB);

            Rp3dContact contact;           
            uint32_t nbContactPoints = contactPair.getNbContactPoints();
            if (nbContactPoints > 0)
            {
                float bestY = -FLT_MAX;
                rp3d::Vector3 bestNormal;
                for (uint32_t i = 0; i < nbContactPoints; i++)
                {
                    // Get contact point
                    CollisionCallback::ContactPoint point = contactPair.getContactPoint(i);

                    // Update contact info
                    const auto& normal = point.getWorldNormal();
                    if (normal.y > bestY)
                    {
                        bestY = normal.y;
                        bestNormal = normal;
                    }
                    contact.penetrationDepth += point.getPenetrationDepth();
                }

                contact.normal = Vector3(bestNormal.x, bestNormal.y, bestNormal.z);
                contact.penetrationDepth /= nbContactPoints;
            }

            pair.contact = contact;

            EventBus::GetInstance().Publish<CollisionCallbackEvent>(pair);
        }
	}

    void PhysicsEventListener::onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData)
    {
        // For each contact pair
        for (uint32_t p = 0; p < callbackData.getNbOverlappingPairs(); p++) {
            // Get the contact pair
            rp3d::OverlapCallback::OverlapPair contactPair = callbackData.getOverlappingPair(p);

            auto* colliderA = static_cast<Rp3dCollider*>(contactPair.getCollider1()->getUserData());
            auto* colliderB = static_cast<Rp3dCollider*>(contactPair.getCollider2()->getUserData());

            EventBus::GetInstance().Publish<CollisionCallbackEvent>(Rp3dCollisionPair(colliderA, colliderB));
        }
    }
}