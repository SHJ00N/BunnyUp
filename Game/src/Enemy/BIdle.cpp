#include "Enemy/BIdle.h"
#include "Enemy/EnemyController.h"

namespace Game
{
    using namespace Behavior;

    BIdle::BIdle(EnemyController* controller) : m_controller(controller)
    {
    }

    NodeState BIdle::Evaluate(float dt)
    {
        if (!m_controller)
        {
            return NodeState::Failure;
        }

        m_controller->ChangeState(m_controller->GetIdle());
        
        return NodeState::Success;
    }
}