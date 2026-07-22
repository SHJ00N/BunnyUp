#pragma once

#include "BehaviorTree/BehaviorNode.h"

namespace Behavior
{
    class Sequence : public BehaviorNode
    {
    public:
        Sequence();
        Sequence(std::vector<std::unique_ptr<BehaviorNode>>&& children);

        // override function
        NodeState Evaluate(float dt) override;
    };
}