#include "Enemy/BCheckTargetInAttackRange.h"
#include "Enemy/EnemyController.h"

namespace Game
{
    using namespace Behavior;

    BCheckTargetInAttackRange::BCheckTargetInAttackRange(EnemyController* controller) : m_controller(controller)
    {
    }

    NodeState BCheckTargetInAttackRange::Evaluate(float dt)
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

        if (m_controller->GetAttack()->IsAttacking())
        {
            return NodeState::Success;
        }

        // Check target in range
        return m_controller->GetAttack()->IsTargetInRange(*m_controller) ? NodeState::Success : NodeState::Failure;
    }
}