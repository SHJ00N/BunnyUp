#include "BehaviorTree/Sequence.h"

namespace Behavior
{
    Sequence::Sequence() : BehaviorNode()
    {
    }
    Sequence::Sequence(std::vector<std::unique_ptr<BehaviorNode>>&& children) : BehaviorNode(std::move(children)) 
    {
    }

    NodeState Sequence::Evaluate(float dt)
    {
        bool anyChildIsRunning = false;

        for (auto& child : children)
        {
            NodeState result = child->Evaluate(dt);

            if (result == NodeState::Failure)
            {
                return NodeState::Failure;
            }

            if (result == NodeState::Running)
            {
                anyChildIsRunning = true;
            }
        }

        state = anyChildIsRunning ? NodeState::Running : NodeState::Success;
        return state;
    }
}