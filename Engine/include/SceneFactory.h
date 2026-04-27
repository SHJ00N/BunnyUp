#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <string>

#include "Singleton.h"

namespace Engine
{
	class Scene;
	using SceneCreateFunction = std::function<std::unique_ptr<Scene>()>;

	class SceneFactory : public Singleton<SceneFactory>
	{
	public:
		void Register(const std::string& name, SceneCreateFunction func)
		{
			m_registry[name] = func;
		}

		std::unique_ptr<Scene> CreateScene(const std::string& name)
		{
			auto it = m_registry.find(name);
			if (it != m_registry.end())
				return it->second();

			return nullptr;
		}

	private:
		std::unordered_map<std::string, SceneCreateFunction> m_registry;
	};
}