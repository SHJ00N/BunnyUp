#pragma once

#include "BehaviorTree/BehaviorNode.h"

namespace Game
{
    class EnemyController;

    class BChaseTarget : public Behavior::BehaviorNode
    {
    public:
        explicit BChaseTarget(EnemyController* controller = nullptr);  // Add enemy controller and tracking target paremeters
        Behavior::NodeState Evaluate(float dt) override;

    private:
        EnemyController* m_controller = nullptr;
    };
}