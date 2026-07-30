#include "Enemy/RangedEnemyController.h"
#include "Enemy/RangedEnemyAttack.h"

namespace Game
{
    void RangedEnemyController::OnEnable()
    {
        m_rangedAttack.Reset();
    }

    void RangedEnemyController::OnUpdate(float dt)
    {
        m_rangedAttack.UpdateTimer(dt);
    }

    void RangedEnemyController::OnDebug()
    {
        m_rangedAttack.DebugDraw(*this);
    }

    void RangedEnemyController::OnGui()
    {
        m_rangedAttack.EditorGui();
    }
}