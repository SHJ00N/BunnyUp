#pragma once

#include <EngineSystem.h>
#include "BehaviorTree/BehaviorNode.h"

namespace Behavior
{
    class BehaviorTree : public Engine::Component
    {
    public:
        // override functions
        void Update(float dt) override;

    protected:
        void OnStart() override;
        virtual std::unique_ptr<BehaviorNode> SetupTree() = 0;  // configure root with children node

    private:
        std::unique_ptr<BehaviorNode> m_root;
    };
}