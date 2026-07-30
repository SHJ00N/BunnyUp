#include "Enemy/MeleeEnemyController.h"
#include "Enemy/MeleeEnemyAttack.h"

namespace Game
{
    void MeleeEnemyController::OnEnable()
    {
        m_meleeAttack.Reset();
    }

    void MeleeEnemyController::OnStart()
    {
        EnemyController::OnStart();

        m_meleeAttack.CreateHitBox(*this);
    }


    void MeleeEnemyController::OnUpdate(float dt)
    {
        m_meleeAttack.UpdateTimer(dt);
    }

    void MeleeEnemyController::OnDebug()
    {
        m_meleeAttack.DebugDraw(*this);
    }

    void MeleeEnemyController::OnGui()
    {
        m_meleeAttack.EditorGui();
    }
}