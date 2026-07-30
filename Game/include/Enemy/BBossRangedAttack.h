#pragma once

#include "BehaviorTree/BehaviorNode.h"

namespace Game
{
    class BossEnemyController;

    class BBossRangedAttack : public Behavior::BehaviorNode
    {
    public:
        explicit BBossRangedAttack(BossEnemyController* controller = nullptr);
        Behavior::NodeState Evaluate(float dt) override;

    private:
        BossEnemyController* m_controller = nullptr;
    };
}