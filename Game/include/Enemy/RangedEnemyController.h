#pragma once

#include "Enemy/EnemyController.h"
#include "Enemy/RangedEnemyAttack.h"

namespace Game
{
    class RangedEnemyController : public EnemyController
    {
    public:
        RangedEnemyAttack* GetRangedAttack() { return &m_rangedAttack; }

    protected:
        void OnEnable() override;
        void OnUpdate(float dt) override;
        void OnDebug() override;
        void OnGui() override;

    private:
        RangedEnemyAttack m_rangedAttack;
    };
}