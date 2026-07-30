#include "pch.h"
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

	void Component::Destroy()
	{
		if (m_isAwake && !m_isDestroyed)
		{
			OnDestroy();
			m_isDestroyed = true;
		}
	}

	void Component::Enable()
	{
		if (!m_isEnabled)
		{
			m_isEnabled = true;
			OnEnable();
		}
	}

	void Component::Disable()
	{
		if (m_isEnabled)
		{
			m_isEnabled = false;
			OnDisable();
		}
	}
}