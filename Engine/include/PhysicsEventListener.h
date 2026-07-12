#pragma once

#include <reactphysics3d/reactphysics3d.h>

namespace Engine
{
	class PhysicsEventListener : public rp3d::EventListener
	{
	public:
		void onContact(const rp3d::CollisionCallback::CallbackData& callbackData) override;
		void onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData) override;
	};
}