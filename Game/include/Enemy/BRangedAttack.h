#pragma once

#include "BehaviorTree/BehaviorNode.h"

namespace Game
{
    class RangedEnemyController;

    class BRangedAttack : public Behavior::BehaviorNode
    {
    public:
        explicit BRangedAttack(RangedEnemyController* controller = nullptr);
        Behavior::NodeState Evaluate(float dt) override;

    private:
        RangedEnemyController* m_controller = nullptr;
    };
}