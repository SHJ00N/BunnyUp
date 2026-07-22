#include "BehaviorTree/Selector.h"

namespace Behavior
{
    Selector::Selector() : BehaviorNode()
    {
    }
    Selector::Selector(std::vector<std::unique_ptr<BehaviorNode>>&& children) : BehaviorNode(std::move(children))
    {
    }

    NodeState Selector::Evaluate(float dt)
    {
        for (auto& child : children)
        {
            NodeState result = child->Evaluate(dt);

            if (result != NodeState::Failure)
            {
                return result;
            }
        }

        state = NodeState::Failure;
        return state;
    }
}