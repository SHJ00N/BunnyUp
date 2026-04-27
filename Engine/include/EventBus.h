#pragma once

#include <unordered_map>
#include <functional>
#include <typeindex>

#include "Singleton.h"

namespace Engine
{
	enum class EventType
	{
		SceneChange
	};

	using EventCallback = std::function<void()>;
	using ListenerID = size_t;

	struct Listener
	{
		ListenerID id;
		EventCallback invoke;
	};

	class EventBus : public Singleton<EventBus>
	{
	public:
		ListenerID Subscribe(EventType type, EventCallback callback);
		void Unsubscribe(EventType type, ListenerID id);
		void Publish(EventType type);
		void Clear();
	private:
		std::unordered_map<EventType, std::vector<Listener>> m_listeners;
		ListenerID m_nextListenerID = 0;
	};
}