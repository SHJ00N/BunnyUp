#pragma once

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Selector.h"
#include "BehaviorTree/Sequence.h"

namespace Game
{
    class BossEnemyController;
    class BossEnemyBehaviorTree : public Behavior::BehaviorTree
    {
    public:
        BossEnemyBehaviorTree(BossEnemyController* controller = nullptr);

    protected:
        std::unique_ptr<Behavior::BehaviorNode> SetupTree() override;

    private:
        BossEnemyController* m_controller = nullptr;
    };
}