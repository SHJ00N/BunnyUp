#pragma once

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Selector.h"
#include "BehaviorTree/Sequence.h"

namespace Game
{
    class RangedEnemyController;
    class RangedEnemyBehaviorTree : public Behavior::BehaviorTree
    {
    public:
        RangedEnemyBehaviorTree(RangedEnemyController* controller = nullptr);

    protected:
        std::unique_ptr<Behavior::BehaviorNode> SetupTree() override;

    private:
        RangedEnemyController* m_controller = nullptr;
    };
}