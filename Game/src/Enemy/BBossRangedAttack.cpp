#include "Enemy/BBossRangedAttack.h"
#include "Enemy/BossEnemyController.h"

namespace Game
{
    using namespace Behavior;

    BBossRangedAttack::BBossRangedAttack(BossEnemyController* controller) : m_controller(controller)
    {
    }

    NodeState BBossRangedAttack::Evaluate(float dt)
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

        auto* attack = m_controller->GetRangedAttack();
        if (attack->IsAttacking())
        {
            return NodeState::Running;
        }

        if (!attack->CanAttack())
        {
            return NodeState::Failure;
        }

        // request attack
        m_controller->ChangeState(attack, false);
        return NodeState::Running;
    }
}