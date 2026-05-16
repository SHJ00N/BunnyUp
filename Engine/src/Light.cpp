#include "Light.h"
#include "Scene.h"

namespace Engine
{
	void Light::OnAwake()
	{
		ownerGameObject->scene->RegisterLight(this);
	}
	void Light::OnDestroy()
	{
		ownerGameObject->scene->UnregisterLight(this);
	}
}