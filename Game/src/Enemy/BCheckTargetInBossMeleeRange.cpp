#include "Enemy/BCheckTargetInBossMeleeRange.h"
#include "Enemy/BossEnemyController.h"

namespace Game
{
    using namespace Behavior;

    BCheckTargetInBossMeleeRange::BCheckTargetInBossMeleeRange(BossEnemyController* controller) : m_controller(controller)
    {
    }

    NodeState BCheckTargetInBossMeleeRange::Evaluate(float dt)
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