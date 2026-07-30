#pragma once

#include "BehaviorTree/BehaviorNode.h"

namespace Game
{
    class MeleeEnemyController;

    class BCheckTargetInMeleeAttackRange : public Behavior::BehaviorNode
    {
    public:
        explicit BCheckTargetInMeleeAttackRange(MeleeEnemyController* controller = nullptr);
        Behavior::NodeState Evaluate(float dt) override;

    private:
        MeleeEnemyController* m_controller = nullptr;
    };
}