#include "Enemy/BossEnemyBehaviorTree.h"
#include "Enemy/BossEnemyController.h"
#include "Enemy/BIdle.h"
#include "Enemy/BChaseTarget.h"
#include "Enemy/BCheckTargetInBossMeleeRange.h"
#include "Enemy/BCheckTargetInBossRangedRange.h"
#include "Enemy/BBossMeleeAttack.h"
#include "Enemy/BBossRangedAttack.h"
#include "Enemy/BDamaged.h"
#include "Enemy/BDeath.h"

namespace Game
{
    using namespace Behavior;

    BossEnemyBehaviorTree::BossEnemyBehaviorTree(BossEnemyController* controller) : m_controller(controller)
    {
    }

    std::unique_ptr<BehaviorNode> BossEnemyBehaviorTree::SetupTree()
    {
        std::vector<std::unique_ptr<BehaviorNode>> children;

        // death node
        children.push_back(std::make_unique<BDeath>(m_controller));

        //// damaged node
        //children.push_back(std::make_unique<BDamaged>(m_controller));

        // ranged attack sequence
        auto rangedAttackSeq = std::vector<std::unique_ptr<BehaviorNode>>();
        rangedAttackSeq.push_back(std::make_unique<BCheckTargetInBossRangedRange>(m_controller));
        rangedAttackSeq.push_back(std::make_unique<BBossRangedAttack>(m_controller));
        children.push_back(std::make_unique<Sequence>(std::move(rangedAttackSeq)));

        // melee attack sequence
        auto meleeAttackSeq = std::vector<std::unique_ptr<BehaviorNode>>();
        meleeAttackSeq.push_back(std::make_unique<BCheckTargetInBossMeleeRange>(m_controller));
        meleeAttackSeq.push_back(std::make_unique<BBossMeleeAttack>(m_controller));
        children.push_back(std::make_unique<Sequence>(std::move(meleeAttackSeq)));

        // chase target node
        children.push_back(std::make_unique<BChaseTarget>(m_controller));

        // idle node
        children.push_back(std::make_unique<BIdle>(m_controller));

        return std::make_unique<Selector>(std::move(children));
    }
}