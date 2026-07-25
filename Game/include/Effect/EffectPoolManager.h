#pragma once

#include <EngineSystem.h>
#include "Singleton.h"
#include "Effect/EffectObject.h"

#include <queue>

namespace Game
{
    class EffectPoolManager : public Singleton<EffectPoolManager>
    {
    public:
        EffectObject* GetPool(EffectObjectType type, Engine::Vector3 position, Engine::Vector3 rotation, Engine::Vector3 scale);
        void ReturnPool(EffectObject* object);

    private:
        std::unordered_map<EffectObjectType, std::queue<EffectObject*>> m_pools;

        EffectObject* createEffect(EffectObjectType type);
    };
}