#include "Enemy/BCheckTargetInBossRangedRange.h"
#include "Enemy/BossEnemyController.h"

namespace Game
{
    using namespace Behavior;

    BCheckTargetInBossRangedRange::BCheckTargetInBossRangedRange(BossEnemyController* controller) : m_controller(controller)
    {
    }

    NodeState BCheckTargetInBossRangedRange::Evaluate(float dt)
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