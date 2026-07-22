#include "BehaviorTree/BehaviorNode.h"

namespace Behavior
{
    BehaviorNode::BehaviorNode(std::vector<std::unique_ptr<BehaviorNode>>&& children)
    {
        for (auto& child : children)
        {
            attach(std::move(child));
        }
    }

    NodeState BehaviorNode::Evaluate(float dt)
    {
        return NodeState::Failure;
    }

    void BehaviorNode::attach(std::unique_ptr<BehaviorNode> node)
    {
        node->parent = this;
        children.push_back(std::move(node));
    }
}