#pragma once

#include "BehaviorTree/BehaviorNode.h"

namespace Game
{
    class EnemyController;
    class BDamaged : public Behavior::BehaviorNode
    {
    public:
        explicit BDamaged(EnemyController* controller = nullptr);
        Behavior::NodeState Evaluate(float dt) override;

    private:
        EnemyController* m_controller = nullptr;
    };
}