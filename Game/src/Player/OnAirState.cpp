#include "Player/PlayerController.h"
#include "Input/PlayerInputManager.h"
#include "Player/OnAirState.h"
#include "Common/Health.h"

namespace Game
{
    bool OnAirState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
    {
        if (playerController.GetHealth()->IsDeath())
        {
            playerController.ChangeState(playerController.GetDeathState());
            return true;
        }


        if (playerController.hasDamage)
        {
            playerController.ChangeState(playerController.GetDamageState());
            return true;
        }

        return false;
    }

    bool OnAirState::IsGround(PlayerController& playerController, const PlayerInputState& inputState)
    {
        if (playerController.isGrounded)
        {
            Vector2 movementInput(0.0f, 0.0f);
            movementInput.x = static_cast<float>(inputState.moveRight - inputState.moveLeft);
            movementInput.y = static_cast<float>(inputState.moveForward - inputState.moveBackward);

            if (Length(movementInput) <= 0.0f)
            {
                if (playerController.isCombat)
                {
                    playerController.ChangeState(playerController.GetCombatState());
                }
                else
                {
                    playerController.ChangeState(playerController.GetIdleState());
                }
            }
            else
            {
                playerController.ChangeState(playerController.GetWalkingState());
            }

            return true;
        }

        return false;
    }

    bool OnAirState::CanAttack(PlayerController& playerController, const PlayerInputState& inputState)
    {
        // Transition to Attack state
        if (inputState.attack)
        {
            playerController.ChangeState(playerController.GetJumpAttackState());
            return true;
        }

        return false;
    }

    void OnAirState::OnUpdate(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState)
    {
        Vector2 movementInput(0.0f, 0.0f);
        movementInput.x = static_cast<float>(inputState.moveRight - inputState.moveLeft);
        movementInput.y = static_cast<float>(inputState.moveForward - inputState.moveBackward);

        auto* rigidbody = playerController.GetRigidbody();
        auto linearVelocity = rigidbody->GetLinearVelocity();
        auto movementDirection = Vector3(movementInput.x, 0.0f, movementInput.y);
        if (Length(movementDirection) > 0.0f)
        {
            movementDirection = Normalize(movementDirection);

            auto velocity = movementDirection * playerController.moveSpeed;
            velocity.y = linearVelocity.y; // Preserve vertical velocity (e.g., for jumping)
            rigidbody->SetLinearVelocity(velocity);

            // Rotate the player to face the movement direction
            auto& transform = playerController.ownerGameObject->transform;
            auto targetRotation = AngleAxis(atan2(movementDirection.x, movementDirection.z) * (180.0f / 3.14159265f), Vector3(0.0f, 1.0f, 0.0f));
            const auto& currentRotation = transform.GetLocalQuaternionRotation();
            auto rotation = Slerp(currentRotation, targetRotation, 10.0f * deltaTime);
            transform.SetLocalRotation(rotation);
        }

        float gravityScale = linearVelocity.y < 0.0f ? 15.0f : 10.0f;
        rigidbody->AddForce(Vector3(0.0f, rigidbody->GetMass() * -9.81f * gravityScale, 0.0f));
    }

    void OnAirState::Exit(PlayerController& playerController)
    {
        auto* rigidbody = playerController.GetRigidbody();
        rigidbody->SetLinearVelocity(Vector3(0.0f));
    }
}