#include "Component.h"

namespace Engine
{
	void Component::Awake()
	{
		if (!m_isAwake)
		{
			m_isAwake = true;
			OnAwake();
		}
	}

	void Component::Start()
	{
		if (!m_isStarted)
		{
			m_isStarted = true;
			OnStart();
		}
	}
}