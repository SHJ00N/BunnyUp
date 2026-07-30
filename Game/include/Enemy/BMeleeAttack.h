#pragma once

#include "BehaviorTree/BehaviorNode.h"

namespace Game
{
    class EnemyController;
    class MeleeEnemyController;

    class BMeleeAttack : public Behavior::BehaviorNode
    {
    public:
        explicit BMeleeAttack(MeleeEnemyController* controller = nullptr);
        Behavior::NodeState Evaluate(float dt) override;

    private:
        MeleeEnemyController* m_controller = nullptr;
    };
}