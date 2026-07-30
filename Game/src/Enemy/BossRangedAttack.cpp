#include "Enemy/BossRangedAttack.h"
#include "Enemy/EnemyController.h"
#include "Common/ObjectPoolManager.h"

namespace Game
{
    using namespace Engine;

    void BossRangedAttack::EnterAttack(EnemyController& controller)
    {
        auto direction = controller.GetTarget()->transform.GetWorldPosition() - controller.ownerGameObject->transform.GetWorldPosition();
        direction.y = 0.0f;
        direction = Normalize(direction);

        float yaw = Degrees(std::atan2(direction.x, direction.z));
        Quaternion rotation = RotationPitchYawRoll(0.0f, yaw, 0.0f);
        controller.ownerGameObject->transform.SetLocalRotation(rotation);

        // set animation to attack anim
        auto* animator = controller.GetAnimator();
        if (animator)
        {
            animator->PlayAnimation("RangedAttack", true);
        }
    }

    void BossRangedAttack::StartAttack(EnemyController& controller)
    {
        auto& transform = controller.ownerGameObject->transform;

        Vector3 position = transform.GetWorldPosition();
        position.y += 5.0f;
        const Vector3 forward = transform.GetForward();
        const Vector3 right = transform.GetRight();

        constexpr float distance = 3.0f;

        const Vector3 directions[] =
        {
            forward,
            forward * -1.0f,
            right,
            right * -1.0f
        };

        for (const auto& direction : directions)
        {
            float angle = std::atan2(direction.x, direction.z);
            float angleDegree = DirectX::XMConvertToDegrees(angle);

            Vector3 rotation(0.0f, angleDegree, 0.0f);

            ObjectPoolManager::GetInstance().GetPool("BigThron", position + direction * distance, rotation, Vector3(0.3f, 0.3f, 0.6f));
        }
    }
}