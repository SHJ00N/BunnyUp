#pragma once

#include "Enemy/EnemyAttack.h"

namespace Game
{
    class EnemyController;
    class EnemyAttackHitBox;

    class BossRangedAttack : public EnemyAttack
    {
    protected:
        void EnterAttack(EnemyController& controller) override;
        void StartAttack(EnemyController& controller) override;
    };
}