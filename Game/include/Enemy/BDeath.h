#pragma once

#include "BehaviorTree/BehaviorNode.h"

namespace Game
{
    class EnemyController;
    class BDeath : public Behavior::BehaviorNode
    {
    public:
        explicit BDeath(EnemyController* controller = nullptr);
        Behavior::NodeState Evaluate(float dt) override;

    private:
        EnemyController* m_controller = nullptr;
    };
}