#pragma once

#include <reactphysics3d/reactphysics3d.h>

namespace Engine
{
	class TriggerEventListener : public rp3d::EventListener
	{
	public:
		void onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData) override;
	};
}