#pragma once

#include <reactphysics3d/reactphysics3d.h>

namespace Engine
{
	class CollisionEventListener : public rp3d::EventListener
	{
	public:
		void onContact(const rp3d::CollisionCallback::CallbackData& callbackData) override;
	};
}