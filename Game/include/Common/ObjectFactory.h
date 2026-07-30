#pragma once

#include <EngineSystem.h>

namespace Game
{
    using ObjectCreateFunction = std::function<Engine::GameObject*(Engine::GameObject*)>;

    class ObjectFactory
    {
    public:
        void Initialize();
        Engine::GameObject* Create(const std::string& name, Engine::GameObject* parent);

    private:
        std::unordered_map<std::string, ObjectCreateFunction> m_registry;

        Engine::GameObject* createSlime(Engine::GameObject* parent);
        Engine::GameObject* createTurtleShell(Engine::GameObject* parent);
        Engine::GameObject* createBoss(Engine::GameObject* parent);
        Engine::GameObject* createThron(Engine::GameObject* parent);
        Engine::GameObject* createBigThron(Engine::GameObject* parent);
    };
}