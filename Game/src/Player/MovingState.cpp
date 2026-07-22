#include "Player/MovingState.h"
#include "Player/PlayerController.h"
#include "Player/IdleState.h"
#include "Input/PlayerInputManager.h"

namespace Game
{
    bool MovingState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
    {
        if (OnGroundState::HandleInput(playerController, inputState)) return true;
        if (CanJump(playerController, inputState)) return true;
        if (CanFall(playerController)) return true;
        if (CanDash(playerController, inputState)) return true;
        if (CanAttack(playerController, inputState)) return true;

        if (!inputState.moveForward && !inputState.moveBackward && !inputState.moveLeft && !inputState.moveRight)
        {
            // Transition to IdleState
            if (playerController.isCombat)
            {
                playerController.ChangeState(playerController.GetCombatState());
            }
            else
            {
                playerController.ChangeState(playerController.GetIdleState());
            }
            return true;
        }

        return false;
    }

    void MovingState::OnUpdate(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState)
    {
        Vector2 movementInput(0.0f, 0.0f);
        movementInput.x = static_cast<float>(inputState.moveRight - inputState.moveLeft);
        movementInput.y = static_cast<float>(inputState.moveForward - inputState.moveBackward);

        auto movementDirection = Vector3(movementInput.x, 0.0f, movementInput.y);
        if (Length(movementDirection) > 0.0f)
        {
            movementDirection = Normalize(movementDirection);

            auto* rigidbody = playerController.GetRigidbody();
            auto linearVelocity = rigidbody->GetLinearVelocity();
            auto velocity = movementDirection * playerController.moveSpeed * (playerController.isRunning ? 2.0f : 1.0f);
            velocity.y = linearVelocity.y; // Preserve vertical velocity (e.g., for jumping)
            rigidbody->SetLinearVelocity(velocity);

            // Rotate the player to face the movement direction
            auto& transform = playerController.ownerGameObject->transform;
            auto targetRotation = AngleAxis(atan2(movementDirection.x, movementDirection.z) * (180.0f / 3.14159265f), Vector3(0.0f, 1.0f, 0.0f));
            const auto& currentRotation = transform.GetLocalQuaternionRotation();
            auto rotation = Slerp(currentRotation, targetRotation, 10.0f * deltaTime);
            transform.SetLocalRotation(rotation);
        }
    }

    void MovingState::Exit(PlayerController& playerController)
    {
        // Stop the player's movement when exiting the moving state
        auto* rigidbody = playerController.GetRigidbody();
        auto linearVelocity = rigidbody->GetLinearVelocity();
        rigidbody->SetLinearVelocity(Vector3(0.0f, linearVelocity.y, 0.0f));
    }
}