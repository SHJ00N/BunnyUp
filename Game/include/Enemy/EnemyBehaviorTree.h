#pragma once

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Selector.h"
#include "BehaviorTree/Sequence.h"

namespace Game
{
    class EnemyController;
    class EnemyBehaviorTree : public Behavior::BehaviorTree
    {
    public:
        EnemyBehaviorTree(EnemyController* controller = nullptr);

    protected:
        std::unique_ptr<Behavior::BehaviorNode> SetupTree() override;

    private:
        EnemyController* m_controller = nullptr;
    };
}