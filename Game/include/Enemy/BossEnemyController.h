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

        void CoolTimeStart() { m_cooldownTimer = m_cooldownTime; }
        bool CanAttack() const { return m_cooldownTimer <= 0.0f; }

    protected:
        void OnEnable() override;
        void OnStart() override;
        void OnUpdate(float dt) override;
        void OnDebug() override;
        void OnGui() override;

    private:
        MeleeEnemyAttack m_meleeAttack;
        BossRangedAttack m_rangedAttack;

        // common cooldown timer for both attacks
        float m_cooldownTimer = 0.0f;
        float m_cooldownTime = 2.0f;
    };
}