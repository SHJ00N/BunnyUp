#pragma once

#include <EngineSystem.h>

namespace Behavior
{
    enum class NodeState
    {
        Success,
        Failure,
        Running
    };

    class BehaviorNode
    {
    public:
        NodeState state = NodeState::Failure;
        BehaviorNode* parent = nullptr;

        // constructors and destructor
        BehaviorNode() = default;
        BehaviorNode(std::vector<std::unique_ptr<BehaviorNode>>&& children);
        virtual ~BehaviorNode() = default;

        virtual NodeState Evaluate(float dt);   // evaulate node state

    protected:
        std::vector<std::unique_ptr<BehaviorNode>> children;

    private:
        void attach(std::unique_ptr<BehaviorNode> node);    // attach node to children
    };
}