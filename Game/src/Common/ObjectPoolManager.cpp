#include "Common/ObjectPoolManager.h"

namespace Game
{
    using namespace Engine;

    ObjectPoolManager::ObjectPoolManager()
    {
        m_factory.Initialize();
    }

    Engine::GameObject* ObjectPoolManager::GetPool(const std::string& name, Engine::Vector3 position, Engine::Vector3 rotation, Engine::Vector3 scale)
    {
        auto& pool = m_pools[name];

        // Get object
        GameObject* object = nullptr;
        if (pool.empty())
        {
            object = m_factory.Create(name, ownerGameObject);
        }
        else
        {
            object = pool.front();
            pool.pop();
        }

        // Set transform
        object->transform.SetLocalPosition(position);
        object->transform.SetLocalRotation(rotation);
        object->transform.SetLocalScale(scale);
        // Set state
        object->Enable();

        return object;
    }

    void ObjectPoolManager::ReturnPool(Engine::GameObject* object)
    {
        object->Disable();
        m_pools[object->GetName()].push(object);
    }
}