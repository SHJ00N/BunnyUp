#include "pch.h"
#include "CollisionEventListener.h"
#include "EventBus.h"
#include "Rp3dCollider.h"
#include "Rp3dCollisionPair.h"

namespace Engine
{
	void CollisionEventListener::onContact(const rp3d::CollisionCallback::CallbackData& callbackData)
	{
        // For each contact pair
        for (uint32_t p = 0; p < callbackData.getNbContactPairs(); p++) {

            // Get the contact pair
            CollisionCallback::ContactPair contactPair = callbackData.getContactPair(p);
            
            auto* colliderA = static_cast<Rp3dCollider*>(contactPair.getCollider1()->getUserData());
            auto* colliderB = static_cast<Rp3dCollider*>(contactPair.getCollider2()->getUserData());

            EventBus::GetInstance().Publish<CollisionCallbackEvent>(Rp3dCollisionPair(colliderA, colliderB));
        }
	}
}