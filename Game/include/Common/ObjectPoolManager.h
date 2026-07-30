#pragma once

#include <EngineSystem.h>
#include "Singleton.h"
#include "Common/ObjectFactory.h"

#include <queue>

namespace Game
{
    class ObjectPoolManager : public Singleton<ObjectPoolManager>
    {
    public:
        ObjectPoolManager();

        Engine::GameObject* GetPool(const std::string& name, Engine::Vector3 position, Engine::Vector3 rotation, Engine::Vector3 scale);
        void ReturnPool(Engine::GameObject* object);

    private:
        std::unordered_map<std::string, std::queue<Engine::GameObject*>> m_pools;

        ObjectFactory m_factory;
    };
}