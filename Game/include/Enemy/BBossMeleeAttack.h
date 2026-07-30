#pragma once

#include "BehaviorTree/BehaviorNode.h"

namespace Game
{
    class BossEnemyController;

    class BBossMeleeAttack : public Behavior::BehaviorNode
    {
    public:
        explicit BBossMeleeAttack(BossEnemyController* controller = nullptr);
        Behavior::NodeState Evaluate(float dt) override;

    private:
        BossEnemyController* m_controller = nullptr;
    };
}