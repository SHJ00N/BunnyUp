#pragma once

#include "BehaviorTree/BehaviorNode.h"

namespace Game
{
    class RangedEnemyController;

    class BCheckTargetInRangedAttackRange : public Behavior::BehaviorNode
    {
    public:
        explicit BCheckTargetInRangedAttackRange(RangedEnemyController* controller = nullptr);
        Behavior::NodeState Evaluate(float dt) override;

    private:
        RangedEnemyController* m_controller = nullptr;
    };
}