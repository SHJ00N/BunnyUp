#pragma once

#include <unordered_map>
#include <functional>
#include <typeindex>

#include "Singleton.h"

namespace Engine
{
	using ListenerID = size_t;

	struct IEvent
	{
		virtual ~IEvent() = default;
	};

	struct SceneChangedEvent : public IEvent
	{
	};

	class GameObject;
	struct ObjectDestroyedEvent : public IEvent
	{
		GameObject* object;
		ObjectDestroyedEvent(GameObject* obj) : object(obj) { }
	};

	class EventBus : public Singleton<EventBus>
	{
	public:
		void Clear();

		template<typename TEvent>
		ListenerID Subscribe(std::function<void(const TEvent&)> callback)
		{
			auto& listeners = m_listeners[typeid(TEvent)];
			// add listener
			ListenerID id = m_nextListenerID++;
			listeners.push_back(
				{
					id,
					[callback](const IEvent& event)
					{
						callback(static_cast<const TEvent&>(event));
					}
				}
			);

			return id;
		}

		template<typename TEvent>
		void Publish(const TEvent& event)
		{
			auto it = m_listeners.find(typeid(TEvent));

			if (it == m_listeners.end())
			{
				return;
			}
			// invoke listener's function
			for (auto& listener : it->second)
			{
				listener.invoke(event);
			}
		}

		void Unsubscribe(std::type_index type, ListenerID id);
		template<typename TEvent>
		void Unsubscribe(ListenerID id)
		{
			Unsubscribe(typeid(TEvent), id);
		}
	private:
		struct Listener
		{
			ListenerID id;
			std::function<void(const IEvent&)> invoke;
		};

		std::unordered_map<std::type_index, std::vector<Listener>> m_listeners;

		ListenerID m_nextListenerID = 0;
	};
}