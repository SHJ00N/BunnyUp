#include "Player/JumpingState.h"
#include "Player/PlayerController.h"
#include "Player/MovingState.h"
#include "Input/PlayerInputManager.h"
#include "Effect/EffectPoolManager.h"

namespace Game
{
    bool JumpingState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
    {
        if (OnAirState::HandleInput(playerController, inputState)) return true;
        if (IsGround(playerController, inputState)) return true;
        if (CanAttack(playerController, inputState)) return true;

        return false;
    }

    void JumpingState::Enter(PlayerController& playerController)
    {
        // LOG_INFO("Entering Jumping State");
        playerController.GetAnimator()->PlayAnimation("Jump_Place");
        playerController.GetRigidbody()->AddImpulse(Vector3(0.0f, playerController.jumpImpulse, 0.0f)); // Apply upward impulse for jump
        playerController.isGrounded = false;

        const auto& transform = playerController.ownerGameObject->transform;
        auto position = transform.GetWorldPosition();
        position.y += 1.0f;
        EffectPoolManager::GetInstance().GetPool(EffectObjectType::Dust, position, Vector3(0.0f), Vector3(20.0f));
    }

    void JumpingState::Exit(PlayerController& playerController)
    {
        OnAirState::Exit(playerController);
        // LOG_INFO("Exiting Jumping State");
    }
}