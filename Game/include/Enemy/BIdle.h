#pragma once

#include "BehaviorTree/BehaviorNode.h"

namespace Game
{
    class EnemyController;

    class BIdle : public Behavior::BehaviorNode
    {
    public:
        explicit BIdle(EnemyController* controller = nullptr);
        Behavior::NodeState Evaluate(float dt) override;

    private:
        EnemyController* m_controller = nullptr;
    };
}