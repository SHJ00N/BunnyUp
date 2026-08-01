#include "Enemy/BBossMeleeAttack.h"
#include "Enemy/BossEnemyController.h"

namespace Game
{
    using namespace Behavior;

    BBossMeleeAttack::BBossMeleeAttack(BossEnemyController* controller) : m_controller(controller)
    {
    }

    NodeState BBossMeleeAttack::Evaluate(float dt)
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

        if (!m_controller->CanAttack())
        {
            return NodeState::Failure;
        }

        if (!attack->CanAttack())
        {
            return NodeState::Failure;
        }

        // request attack
        m_controller->ChangeState(attack, false);
        m_controller->CoolTimeStart();
        return NodeState::Running;
    }
}