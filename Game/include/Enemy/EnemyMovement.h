#pragma once

#include <EngineSystem.h>
#include "Enemy/EnemyState.h"

namespace Game
{
    class EnemyController;
    class AStar;
    class EnemyMovement : public EnemyState
    {
    public:
        // override function
        void Enter(EnemyController& controller) override;
        void Exit(EnemyController& controller) override;

        void UpdatePath(EnemyController& controller, AStar& aStar);  // find path
        void Stop(EnemyController& controller);
        bool IsArrived() const { return m_isArrived; }
        
        void DebugDraw(EnemyController& controller) const;
        void EditorGui();
    protected:
        void OnUpdate(EnemyController& controller, float dt) override;

    private:
        float m_moveSpeed = 7.5f;
        bool m_isArrived = false;

        std::vector<Engine::Vector3> m_path;
        int m_currentWaypoint = 0;  // path index

        Engine::Vector3 m_lastDestination;

        float m_repathDistance = 12.5f;
        float m_repathDelta = 2.5f;
        float m_waypointReachDistance = 5.0f;   // check arrived
    };
}