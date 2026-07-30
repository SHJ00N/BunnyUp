#pragma once

#include "Enemy/EnemyController.h"
#include "Enemy/MeleeEnemyAttack.h"

namespace Game
{
    class MeleeEnemyController : public EnemyController
    {
    public:
        MeleeEnemyAttack* GetMeleeAttack() { return &m_meleeAttack; }

    protected:
        void OnEnable() override;
        void OnStart() override;
        void OnUpdate(float dt) override;
        void OnDebug() override;
        void OnGui() override;

    private:
        MeleeEnemyAttack m_meleeAttack;
    };
}