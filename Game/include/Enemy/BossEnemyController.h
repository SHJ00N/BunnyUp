#pragma once

#include "Enemy/EnemyController.h"
#include "Enemy/MeleeEnemyAttack.h"
#include "Enemy/BossRangedAttack.h"


namespace Game
{
    class BossEnemyController : public EnemyController
    {
    public:
        MeleeEnemyAttack* GetMeleeAttack() { return &m_meleeAttack; }
        BossRangedAttack* GetRangedAttack() { return &m_rangedAttack; }

    protected:
        void OnEnable() override;
        void OnStart() override;
        void OnUpdate(float dt) override;
        void OnDebug() override;
        void OnGui() override;

    private:
        MeleeEnemyAttack m_meleeAttack;
        BossRangedAttack m_rangedAttack;
    };
}