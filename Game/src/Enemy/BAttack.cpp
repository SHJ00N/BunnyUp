#include "Enemy/BAttack.h"
#include "Enemy/EnemyController.h"

namespace Game
{
    using namespace Behavior;

    BAttack::BAttack(EnemyController* controller) : m_controller(controller)
    {
    }

    NodeState BAttack::Evaluate(float dt)
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

        auto* attack = m_controller->GetAttack();
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