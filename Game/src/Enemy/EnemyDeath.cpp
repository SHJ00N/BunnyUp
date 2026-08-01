#include "Enemy/EnemyDeath.h"
#include "Enemy/EnemyController.h"
#include "Common/ObjectPoolManager.h"

namespace Game
{
    void EnemyDeath::OnUpdate(EnemyController& controller, float dt)
    {
        if (Elapsed(3.0f))
        {
            ObjectPoolManager::GetInstance().ReturnPool(controller.ownerGameObject);
            ResetTimer();
        }
    }

    void EnemyDeath::Enter(EnemyController& controller)
    {
        ResetTimer();
        controller.GetAnimator()->PlayAnimation("Death", true);
    }
}