#pragma once

#include "BehaviorTree/BehaviorNode.h"

namespace Game
{
    class BossEnemyController;

    class BCheckTargetInBossMeleeRange : public Behavior::BehaviorNode
    {
    public:
        explicit BCheckTargetInBossMeleeRange(BossEnemyController* controller = nullptr);
        Behavior::NodeState Evaluate(float dt) override;

    private:
        BossEnemyController* m_controller = nullptr;
    };
}