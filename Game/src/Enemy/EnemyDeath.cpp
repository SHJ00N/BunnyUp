#include "Enemy/EnemyDeath.h"
#include "Enemy/EnemyController.h"

namespace Game
{
    void EnemyDeath::OnUpdate(EnemyController& controller, float dt)
    {
        if (Elapsed(3.0f))
        {
            controller.ownerGameObject->Destroy();
        }
    }

    void EnemyDeath::Enter(EnemyController& controller)
    {
        ResetTimer();
        controller.GetAnimator()->PlayAnimation("Death", true);
    }
}