#include "Enemy/EnemyIdle.h"
#include "Enemy/EnemyController.h"

namespace Game
{
    using namespace Engine;

    void EnemyIdle::Enter(EnemyController& controller)
    {
        auto* animator = controller.GetAnimator();
        if (animator)
        {
            animator->PlayAnimation("Idle");
        }
    }

    void EnemyIdle::OnUpdate(EnemyController& controller, float dt)
    {
        auto* enemyRigidbody = controller.GetRigidbody();
        if (enemyRigidbody)
        {
            // rotation
            auto* target = controller.GetTarget();
            if (target)
            {
                auto& transform = controller.ownerGameObject->transform;
                auto targetDir = Normalize(target->transform.GetWorldPosition() - transform.GetWorldPosition());
                auto targetRotation = AngleAxis(atan2(targetDir.x, targetDir.z) * (180.0f / 3.14159265f), Vector3(0.0f, 1.0f, 0.0f));
                const auto& currentRotation = transform.GetLocalQuaternionRotation();
                auto rotation = Slerp(currentRotation, targetRotation, 0.005f);
                transform.SetLocalRotation(rotation);
            }
        }
    }
}