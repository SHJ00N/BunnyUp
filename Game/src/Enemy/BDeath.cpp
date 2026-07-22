#include "Enemy/BDeath.h"
#include "Enemy/EnemyController.h"
#include "Common/Health.h"

namespace Game
{
    using namespace Behavior;

    BDeath::BDeath(EnemyController* controller) : m_controller(controller)
    {
    }

    NodeState BDeath::Evaluate(float dt)
    {
        if (!m_controller)
        {
            return NodeState::Failure;
        }

        if (!m_controller->GetHealth()->IsDeath())
        {
            return NodeState::Failure;
        }

        m_controller->ChangeState(m_controller->GetDeath());
        return NodeState::Success;
    }
}