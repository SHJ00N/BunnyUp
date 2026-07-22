#include "Enemy/EnemyDamaged.h"
#include "Enemy/EnemyController.h"

namespace Game
{
    void EnemyDamaged::OnUpdate(EnemyController& controller, float dt)
    {
        if (Elapsed(0.5f))
        {
            m_hasDamage = false;
        }
    }

    void EnemyDamaged::Enter(EnemyController& controller)
    {
        ResetTimer();
        controller.GetAnimator()->PlayAnimation("Damage", true);
    }

    void EnemyDamaged::Exit(EnemyController& controller)
    {
    }
}