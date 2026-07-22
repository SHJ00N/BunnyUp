#pragma once

#include "BehaviorTree/BehaviorNode.h"

namespace Game
{
    class EnemyController;

    class BAttack : public Behavior::BehaviorNode
    {
    public:
        explicit BAttack(EnemyController* controller = nullptr);
        Behavior::NodeState Evaluate(float dt) override;

    private:
        EnemyController* m_controller = nullptr;
    };
}