#include "Enemy/BChaseTarget.h"
#include "Enemy/EnemyController.h"

namespace Game
{
    using namespace Behavior;

    BChaseTarget::BChaseTarget(EnemyController* controller) : m_controller(controller)
    {
    }

    NodeState BChaseTarget::Evaluate(float dt)
    {
        if (!m_controller)
        {
            return NodeState::Failure;
        }

        auto* target = m_controller->GetTarget();
        if (!target)
        {
            return NodeState::Failure;
        }

        auto* movement = m_controller->GetMovement();
        if (movement->IsArrived())
        {
            return NodeState::Failure;
        }

        // request chasing to target
        m_controller->ChangeState(movement);

        return NodeState::Running;
    }
}