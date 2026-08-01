#pragma once

#include <EngineSystem.h>
#include "Singleton.h"

#include <queue>

namespace Game
{
    class EnemySpawnManager : public Singleton<EnemySpawnManager>
    {
    public:
        void Update(float dt) override;

        void SetTarget(Engine::GameObject* target) { m_target = target; }
        void AddWayPoint(Engine::GameObject* waypoint);

        int GetMaxSpawnCount() const { return m_maxSpawnCount; }

    private:
        Engine::GameObject* m_target = nullptr;
        std::vector<Engine::GameObject*> m_waypoints;

        int m_spawnCount = 0;
        int m_maxSpawnCount = 15;
        float m_spawnTime = 0.0f;
        bool m_isBossSpawned = false;
    };
}