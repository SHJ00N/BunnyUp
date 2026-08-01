#include "Enemy/BossEnemyController.h"

namespace Game
{
    void BossEnemyController::OnEnable()
    {
        m_meleeAttack.Reset();
        m_rangedAttack.Reset();
    }

    void BossEnemyController::OnStart()
    {
        EnemyController::OnStart();

        m_meleeAttack.CreateHitBox(*this);
    }


    void BossEnemyController::OnUpdate(float dt)
    {
        m_cooldownTimer -= dt;
        m_meleeAttack.UpdateTimer(dt);
        m_rangedAttack.UpdateTimer(dt);
    }

    void BossEnemyController::OnDebug()
    {
        m_meleeAttack.DebugDraw(*this);
        m_rangedAttack.DebugDraw(*this);
    }

    void BossEnemyController::OnGui()
    {
        m_rangedAttack.EditorGui();
        m_meleeAttack.EditorGui();
    }
}