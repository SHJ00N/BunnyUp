#include "Enemy/BDamaged.h"
#include "Enemy/EnemyController.h"

namespace Game
{
    using namespace Behavior;

    BDamaged::BDamaged(EnemyController* controller) : m_controller(controller)
    {
    }

    NodeState BDamaged::Evaluate(float dt)
    {
        if (!m_controller)
        {
            return NodeState::Failure;
        }

        auto* damaged = m_controller->GetDamaged();

        if (!damaged->IsDamaged())
        {
            return NodeState::Failure;
        }

        if (m_controller->GetCurrentState() == damaged)
        {
            return NodeState::Running;
        }

        m_controller->ChangeState(damaged);
        return NodeState::Success;
    }
}