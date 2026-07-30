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

    private:
        Engine::GameObject* m_target = nullptr;
        std::vector<Engine::GameObject*> m_waypoints;

        int m_spawnCount = 0;
        float m_spawnTime = 0.0f;
        bool m_spawnBoss = false;
    };
}