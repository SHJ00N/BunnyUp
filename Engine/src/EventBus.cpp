#include "EventBus.h"

#include <algorithm>

namespace Engine
{
	ListenerID EventBus::Subscribe(EventType type, EventCallback callback)
	{
		auto& listeners = m_listeners[type];
		// add listener
		ListenerID id = m_nextListenerID++;
		listeners.push_back({ id, callback });

		return id;
	}

	void EventBus::Unsubscribe(EventType type, ListenerID id)
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
	}

	void EventBus::Publish(EventType type)
	{
		// Check exists this type
		auto it = m_listeners.find(type);
		if (it == m_listeners.end())
		{
			return;
		}

		// invoke listener's function
		auto& listeners = it->second;
		for (auto& listener : listeners)
		{
			listener.invoke();
		}
	}

	void EventBus::Clear()
	{
		m_listeners.clear();
		m_nextListenerID = 0;
	}
}