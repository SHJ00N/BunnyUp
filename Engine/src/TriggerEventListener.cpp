#include "pch.h"
#include "TriggerEventListener.h"
#include "EventBus.h"
#include "Rp3dCollider.h"
#include "Rp3dCollisionPair.h"

namespace Engine
{
	void TriggerEventListener::onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData)
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