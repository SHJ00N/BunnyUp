#include "Enemy/BCheckTargetInMeleeAttackRange.h"
#include "Enemy/MeleeEnemyController.h"

namespace Game
{
    using namespace Behavior;

    BCheckTargetInMeleeAttackRange::BCheckTargetInMeleeAttackRange(MeleeEnemyController* controller) : m_controller(controller)
    {
    }

    NodeState BCheckTargetInMeleeAttackRange::Evaluate(float dt)
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

        if (m_controller->GetMeleeAttack()->IsAttacking())
        {
            return NodeState::Success;
        }

        // Check target in range
        return m_controller->GetMeleeAttack()->IsTargetInRange(*m_controller) ? NodeState::Success : NodeState::Failure;
    }
}