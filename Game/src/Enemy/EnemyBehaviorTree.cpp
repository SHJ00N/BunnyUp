#include "Enemy/EnemyBehaviorTree.h"
#include "Enemy/BIdle.h"
#include "Enemy/BChaseTarget.h"
#include "Enemy/BCheckTargetInAttackRange.h"
#include "Enemy/BAttack.h"
#include "Enemy/BDamaged.h"
#include "Enemy/BDeath.h"

namespace Game
{
    using namespace Behavior;

    EnemyBehaviorTree::EnemyBehaviorTree(EnemyController* controller) : m_controller(controller)
    {
    }

    std::unique_ptr<BehaviorNode> EnemyBehaviorTree::SetupTree()
    {
        std::vector<std::unique_ptr<BehaviorNode>> children;

        // death node
        children.push_back(std::make_unique<BDeath>(m_controller));

        // damaged node
        children.push_back(std::make_unique<BDamaged>(m_controller));

        // attack sequence
        auto attackSeq = std::vector<std::unique_ptr<BehaviorNode>>();
        attackSeq.push_back(std::make_unique<BCheckTargetInAttackRange>(m_controller));
        attackSeq.push_back(std::make_unique<BAttack>(m_controller));
        children.push_back(std::make_unique<Sequence>(std::move(attackSeq)));

        // chase target node
        children.push_back(std::make_unique<BChaseTarget>(m_controller));

        // idle node
        children.push_back(std::make_unique<BIdle>(m_controller));

        return std::make_unique<Selector>(std::move(children));
    }
}