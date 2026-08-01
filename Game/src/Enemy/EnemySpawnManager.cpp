#include "Enemy/EnemySpawnManager.h"
#include "Common/ObjectPoolManager.h"
#include "Enemy/EnemyController.h"

namespace Game
{
    using namespace Engine;
    void EnemySpawnManager::Update(float dt)
    {
        if (m_waypoints.empty()) return;
        if (m_isBossSpawned) return;

        m_spawnTime -= dt;
        if (m_spawnTime > 0.0f) return;
        
        // spawn
        int wpIndex = Random::Range(0, std::max(0, static_cast<int>(m_waypoints.size()) - 2));
        if (m_spawnCount < m_maxSpawnCount - 1)
        {
            std::string type = Random::Range(0, 1) ? "Slime" : "TurtleShell";
            Vector3 position = m_waypoints[wpIndex]->transform.GetWorldPosition();
            auto enemy = ObjectPoolManager::GetInstance().GetPool(type, position, Vector3(0.0f, 180.0f, 0.0f), Vector3(0.1f));
            m_spawnCount++;

            auto controller = enemy->GetComponent<EnemyController>();
            if (controller)
            {
                controller->SetTarget(m_target);
            }
        }
        else
        {
            Vector3 position = m_waypoints.back()->transform.GetWorldPosition();
            auto boss = ObjectPoolManager::GetInstance().GetPool("Boss", position, Vector3(0.0f, 180.0f, 0.0f), Vector3(0.3f));

            auto controller = boss->GetComponent<EnemyController>();
            if (controller)
            {
                controller->SetTarget(m_target);
            }

            m_isBossSpawned = true;
        }

        m_spawnTime = m_spawnCount == m_maxSpawnCount ? 20.0f : Random::Range(1.0f, 5.0f);
    }

    void EnemySpawnManager::AddWayPoint(GameObject* waypoint)
    {
        m_waypoints.push_back(waypoint);
    }
}