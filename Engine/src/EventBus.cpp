#include "EventBus.h"

#include <algorithm>

namespace Engine
{
	void EventBus::Unsubscribe(std::type_index type, ListenerID id)
	{
		// Check exists this type
		auto it = m_listeners.find(type);
		if (it == m_listeners.end())
		{
			return;
		}

		auto& listeners = it->second;
		listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
			[id](const Listener& l)
			{
				return l.id == id;
			}),
			listeners.end()
		);

		if (listeners.empty())
		{
			m_listeners.erase(it);
		}
	}

	void EventBus::Clear()
	{
		m_listeners.clear();
		m_nextListenerID = 0;
	}
}