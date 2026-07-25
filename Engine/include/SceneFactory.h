#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <string>

#include "Singleton.h"
#include "Scene.h"

namespace Engine
{
	using SceneCreateFunction = std::function<std::unique_ptr<Scene>()>;

	class SceneFactory : public Singleton<SceneFactory>
	{
	public:
		void Register(const std::string& name, SceneCreateFunction func)
		{
			m_registryIndex.push_back(name);
			m_registry[name] = func;
		}

		std::unique_ptr<Scene> CreateScene(const std::string& name)
		{
			auto it = m_registry.find(name);
			if (it != m_registry.end())
				return it->second();

			return nullptr;
		}

		std::unique_ptr<Scene> CreateScene(int index)
		{
			if (index < m_registryIndex.size())
				return m_registry[m_registryIndex[index]]();

			return nullptr;
		}

		int GetSceneCount() const { return static_cast<int>(m_registryIndex.size()); }
		std::string GetSceneName(int index)
		{
			assert(index < m_registryIndex.size() && "Index is out of range");
			return m_registryIndex[index];
		}

	private:
		std::unordered_map<std::string, SceneCreateFunction> m_registry;
		std::vector<std::string> m_registryIndex;
	};
}