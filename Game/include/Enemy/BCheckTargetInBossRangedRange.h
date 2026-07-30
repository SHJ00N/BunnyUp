#pragma once

#include "BehaviorTree/BehaviorNode.h"

namespace Game
{
    class BossEnemyController;

    class BCheckTargetInBossRangedRange : public Behavior::BehaviorNode
    {
    public:
        explicit BCheckTargetInBossRangedRange(BossEnemyController* controller = nullptr);
        Behavior::NodeState Evaluate(float dt) override;

    private:
        BossEnemyController* m_controller = nullptr;
    };
}