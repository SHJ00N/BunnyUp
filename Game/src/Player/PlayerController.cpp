#include "Player/PlayerController.h"
#include "Player/PlayerState.h"
#include "Input/PlayerInputManager.h"
#include "Player/IdleState.h"

namespace Game
{
    void PlayerController::OnStart()
    {
        m_animator = ownerGameObject->GetComponent<Animator>();
        m_rigidbody = ownerGameObject->GetComponent<Rp3dRigidbody>();
        m_collider = ownerGameObject->GetComponent<Rp3dCapsuleCollider>();

        m_rigidbody->SetLinearDamping(0.3f);
    }

    void PlayerController::Update(float dt)
    {
        if (m_currentState == nullptr)
        {
            m_currentState = &m_idleState;
            m_currentState->Enter(*this);
        }

        PlayerInputState inputState = PlayerInputManager::GetInstance().GetInputState();

        m_currentState->HandleInput(*this, inputState);
        m_currentState->Update(dt, *this, inputState);
    }

    void PlayerController::FixedUpdate(float fdt)
    {
        // bool previousGrounded = isGrounded;

        //RaycastHit hitInfo;
        //Vector3 rayOrigin = ownerGameObject->transform.GetWorldPosition() + m_collider->GetLocalPosition();
        //rayOrigin.y -= m_collider->GetHeight() * 0.5f + m_collider->GetRadius();
        //rayOrigin.y += 0.02f; // Slightly above the player's position to avoid self-collision
        //
        //float offset = 1.0f;
        //Vector3 rayL = { rayOrigin.x - offset, rayOrigin.y, rayOrigin.z };
        //Vector3 rayR = { rayOrigin.x + offset, rayOrigin.y, rayOrigin.z };
        //Vector3 rayU = { rayOrigin.x, rayOrigin.y, rayOrigin.z + offset };
        //Vector3 rayD = { rayOrigin.x, rayOrigin.y, rayOrigin.z - offset };
        //Vector3 rayLU = { rayOrigin.x - offset, rayOrigin.y, rayOrigin.z + offset };
        //Vector3 rayRU = { rayOrigin.x + offset, rayOrigin.y, rayOrigin.z + offset };
        //Vector3 rayLD = { rayOrigin.x - offset, rayOrigin.y, rayOrigin.z - offset };
        //Vector3 rayRD = { rayOrigin.x + offset, rayOrigin.y, rayOrigin.z - offset };

        //
        //Vector3 rayNormal = Vector3(0.0f, -1.0f, 0.0f);
        //float distance = 1.0f;
        //// Perform a raycast downwards to check if the player is grounded
        //bool hit =
        //    ownerGameObject->scene->GetPhysicsSystem()->Raycast(rayOrigin, rayNormal, distance, hitInfo);
        //    ownerGameObject->scene->GetPhysicsSystem()->Raycast(rayL, rayNormal, distance, hitInfo) ||
        //    ownerGameObject->scene->GetPhysicsSystem()->Raycast(rayR, rayNormal, distance, hitInfo) ||
        //    ownerGameObject->scene->GetPhysicsSystem()->Raycast(rayU, rayNormal, distance, hitInfo) ||
        //    ownerGameObject->scene->GetPhysicsSystem()->Raycast(rayD, rayNormal, distance, hitInfo) ||
        //    ownerGameObject->scene->GetPhysicsSystem()->Raycast(rayLU, rayNormal, distance, hitInfo) ||
        //    ownerGameObject->scene->GetPhysicsSystem()->Raycast(rayRU, rayNormal, distance, hitInfo) ||
        //    ownerGameObject->scene->GetPhysicsSystem()->Raycast(rayLD, rayNormal, distance, hitInfo) ||
        //    ownerGameObject->scene->GetPhysicsSystem()->Raycast(rayRD, rayNormal, distance, hitInfo);

        //Vector3 velocity = m_rigidbody->GetLinearVelocity();
        //bool detectedGround = hit && isFootGrounded && velocity.y < 0.5f;    // if enter the jumping state, prevent grounded true

        //if (detectedGround)
        //{
        //    m_groundLostTimer = 0.0f;
        //    isGrounded = true;
        //}
        //else
        //{
        //    m_groundLostTimer += fdt;

        //    if (m_groundLostTimer >= GroundLostDelay)
        //    {
        //        isGrounded = false;
        //    }
        //}

        // update coyote
        if (isGrounded)
        {
            m_coyoteTimer = CoyoteTime;
        }
        else
        {
            m_coyoteTimer -= fdt;
        }
    }

    void PlayerController::ChangeState(PlayerState* nextState)
    {
        if(m_currentState == nextState || nextState == nullptr)
            return;

        m_currentState->Exit(*this);
        m_currentState = nextState;
        m_currentState->Enter(*this);
    }

    void PlayerController::OnCollisionEnter(Rp3dCollider* other)
    {
        if (other->ownerGameObject->GetTag() == ObjectTag::Ground)
        {
            LOG_INFO("Ground Hit!");
        }
    }

    void PlayerController::OnCollisionExit(Rp3dCollider* other)
    {

    }

    void PlayerController::OnImGui()
    {
        ImGui::DragFloat("Speed", &moveSpeed, 0.1f, 0.0f, 100.0f);
    }
}