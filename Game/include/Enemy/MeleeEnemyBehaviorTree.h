#pragma once

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Selector.h"
#include "BehaviorTree/Sequence.h"

namespace Game
{
    class MeleeEnemyController;
    class MeleeEnemyBehaviorTree : public Behavior::BehaviorTree
    {
    public:
        MeleeEnemyBehaviorTree(MeleeEnemyController* controller = nullptr);

    protected:
        std::unique_ptr<Behavior::BehaviorNode> SetupTree() override;

    private:
        MeleeEnemyController* m_controller = nullptr;
    };
}