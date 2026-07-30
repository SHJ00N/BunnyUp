#include "Enemy/BMeleeAttack.h"
#include "Enemy/EnemyController.h"
#include "Enemy/MeleeEnemyController.h"

namespace Game
{
    using namespace Behavior;

    BMeleeAttack::BMeleeAttack(MeleeEnemyController* controller) : m_controller(controller)
    {
    }

    NodeState BMeleeAttack::Evaluate(float dt)
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

        auto* attack = m_controller->GetMeleeAttack();
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