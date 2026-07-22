#include "Enemy/EnemyMovement.h"
#include "Enemy/NavigationManager.h"
#include "Enemy/EnemyController.h"
#include "Enemy/AStar.h"

namespace Game
{
    using namespace Engine;

    void EnemyMovement::Enter(EnemyController& controller)
    {
        // set animation
        auto* enemyAnimator = controller.GetAnimator();
        if (enemyAnimator)
        {
            enemyAnimator->PlayAnimation("Walk");
        }
    }

    void EnemyMovement::Exit(EnemyController& controller)
    {
        auto* enemyRigidbody = controller.GetRigidbody();
        if (enemyRigidbody)
        {
            auto linearVelocity = enemyRigidbody->GetLinearVelocity();
            enemyRigidbody->SetLinearVelocity(Vector3(0.0f, linearVelocity.y, 0.0f));
        }
    }

    void EnemyMovement::OnUpdate(EnemyController& controller, float dt)
    {
        auto* enemy = controller.ownerGameObject;

        // if arrived to last waypoint, enemy stop
        if (m_currentWaypoint >= m_path.size())
        {
            m_isArrived = true;
            return;
        }

        // calculate move direction
        Vector3 pathPos = m_path[m_currentWaypoint];
        Vector3 enemyPos = controller.ownerGameObject->transform.GetWorldPosition();
        Vector3 direction = pathPos - enemyPos;
        direction.y = 0.0f;

        // check arrived
        if (LengthSq(direction) < m_waypointReachDistance * m_waypointReachDistance)
        {
            ++m_currentWaypoint;

            if (m_currentWaypoint < m_path.size())
            {
                pathPos = m_path[m_currentWaypoint];
                direction = pathPos - enemyPos;
            }
        }

        // move
        direction = Normalize(direction);
        auto* enemyRigidbody = controller.GetRigidbody();
        if (enemyRigidbody)
        {
            // velocity
            auto linearVelocity = enemyRigidbody->GetLinearVelocity();
            auto velocity = direction * m_moveSpeed;
            velocity.y = linearVelocity.y;
            enemyRigidbody->SetLinearVelocity(velocity);
            // rotation
            auto& transform = controller.ownerGameObject->transform;
            auto targetRotation = AngleAxis(atan2(direction.x, direction.z) * (180.0f / 3.14159265f), Vector3(0.0f, 1.0f, 0.0f));
            const auto& currentRotation = transform.GetLocalQuaternionRotation();
            auto rotation = Slerp(currentRotation, targetRotation, 0.01f);
            transform.SetLocalRotation(rotation);
        }
    }

    void EnemyMovement::UpdatePath(EnemyController& controller, AStar& aStar)
    {
        auto target = controller.GetTarget();
        if (!target) return;

        auto* enemy = controller.ownerGameObject;
        const auto targetPos = target->transform.GetWorldPosition();
        
        auto targetDelta = targetPos - m_lastDestination;
        targetDelta.y = 0.0f;
        auto targetDistance = targetPos - enemy->transform.GetWorldPosition();
        targetDistance.y = 0.0f;
        
        if ((LengthSq(targetDistance) > m_repathDistance * m_repathDistance) && (LengthSq(targetDelta) > m_repathDelta * m_repathDelta))
        {
            m_lastDestination = targetPos;

            // get cells coordinate about target
            std::vector<GridCoord> goals;
            auto targetCollider = target->GetComponent<Rp3dCollider>();
            if (targetCollider)
            {
                goals = NavigationManager::GetInstance().GetGrid().GetContactCoordsForBounds(targetCollider->GetBounds());
            }
            else
            {
                goals.push_back(NavigationManager::GetInstance().GetGrid().WorldToCell(targetPos));
            }

            // search path
            auto path = aStar.FindPath(NavigationManager::GetInstance().GetGrid(), enemy->transform.GetWorldPosition(), goals);
            if (!path.empty())
            {
                m_path = path;
                m_currentWaypoint = 1;
                m_isArrived = false;
            }
        }
    }

    void EnemyMovement::Stop(EnemyController& controller)
    {
        auto* enemyRigidbody = controller.GetRigidbody();
        if (enemyRigidbody)
        {
            auto linearVelocity = enemyRigidbody->GetLinearVelocity();
            enemyRigidbody->SetLinearVelocity(Vector3(0.0f, linearVelocity.y, 0.0f));
        }
    }

    void EnemyMovement::DebugDraw(EnemyController& controller) const
    {
        for (int i = std::max(0, m_currentWaypoint - 1); i < m_path.size() - 1; ++i)
        {
            Vector3 p0 = m_path[i];
            Vector3 p1 = m_path[i + 1];
            Vector3 pos = controller.ownerGameObject->transform.GetWorldPosition();
            p0.y = pos.y + 5.0f;
            p1.y = pos.y + 5.0f;
            DebugRenderer::GetInstance().AddLine(p0, p1, Vector4(1.0f, 1.0f, 0.0f, 1.0f));
        }
    }

    void EnemyMovement::EditorGui()
    {
        ImGui::DragFloat("Speed", &m_moveSpeed, 0.1f, 0.0f, 100.0f);
    }
}