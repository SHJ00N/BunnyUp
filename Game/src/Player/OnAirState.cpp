#include "Player/PlayerController.h"
#include "Input/PlayerInputManager.h"
#include "Player/OnAirState.h"

namespace Game
{
    bool OnAirState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
    {
        if (playerController.isGrounded)
        {
            Vector2 movementInput(0.0f, 0.0f);
            movementInput.x = inputState.moveRight - inputState.moveLeft;
            movementInput.y = inputState.moveForward - inputState.moveBackward;

            if (Length(movementInput) <= 0.0f)
            {
                playerController.ChangeState(playerController.GetIdleState());
            }
            else
            {
                playerController.ChangeState(playerController.GetWalkingState());
            }

            return true;
        }

        return false;
    }

    void OnAirState::Update(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState)
    {
        Vector2 movementInput(0.0f, 0.0f);
        movementInput.x = inputState.moveRight - inputState.moveLeft;
        movementInput.y = inputState.moveForward - inputState.moveBackward;

        auto* rigidbody = playerController.GetRigidbody();
        auto linearVelocity = rigidbody->GetLinearVelocity();
        auto movementDirection = Vector3(movementInput.x, 0.0f, movementInput.y);
        if (Length(movementDirection) > 0.0f)
        {
            movementDirection = Normalize(movementDirection);

            auto velocity = movementDirection * playerController.moveSpeed * 0.75f;
            velocity.y = linearVelocity.y; // Preserve vertical velocity (e.g., for jumping)
            rigidbody->SetLinearVelocity(velocity);

            // Rotate the player to face the movement direction
            auto& transform = playerController.ownerGameObject->transform;
            auto targetRotation = AngleAxis(atan2(movementDirection.x, movementDirection.z) * (180.0f / 3.14159265f), Vector3(0.0f, 1.0f, 0.0f));
            const auto& currentRotation = transform.GetLocalQuaternionRotation();
            auto rotation = Slerp(currentRotation, targetRotation, 10.0f * deltaTime);
            transform.SetLocalRotation(rotation);
        }

        float gravityScale = linearVelocity.y < 0.0f ? 8.0f : 5.0f;
        rigidbody->AddForce(Vector3(0.0f, rigidbody->GetMass() * -9.81f * gravityScale, 0.0f));
    }

    void OnAirState::Exit(PlayerController& playerController)
    {
        auto* rigidbody = playerController.GetRigidbody();
        rigidbody->SetLinearVelocity(Vector3(0.0f));
    }
}