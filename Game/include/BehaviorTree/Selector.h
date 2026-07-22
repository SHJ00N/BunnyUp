#pragma once

#include "BehaviorTree/BehaviorNode.h"

namespace Behavior
{
    class Selector : public BehaviorNode
    {
    public:
        Selector();
        Selector(std::vector<std::unique_ptr<BehaviorNode>>&& children);

        // override function
        NodeState Evaluate(float dt) override;
    };
}