#pragma once

#include "BehaviorTree/BehaviorNode.h"

namespace Game
{
    class EnemyController;

    class BCheckTargetInAttackRange : public Behavior::BehaviorNode
    {
    public:
        explicit BCheckTargetInAttackRange(EnemyController* controller = nullptr);
        Behavior::NodeState Evaluate(float dt) override;

    private:
        EnemyController* m_controller = nullptr;
    };
}