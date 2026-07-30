#include "Enemy/BCheckTargetInRangedAttackRange.h"
#include "Enemy/RangedEnemyController.h"

namespace Game
{
    using namespace Behavior;

    BCheckTargetInRangedAttackRange::BCheckTargetInRangedAttackRange(RangedEnemyController* controller) : m_controller(controller)
    {
    }

    NodeState BCheckTargetInRangedAttackRange::Evaluate(float dt)
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

        if (m_controller->GetRangedAttack()->IsAttacking())
        {
            return NodeState::Success;
        }

        // Check target in range
        return m_controller->GetRangedAttack()->IsTargetInRange(*m_controller) ? NodeState::Success : NodeState::Failure;
    }
}