#include "Player/PlayerController.h"
#include "Player/PlayerState.h"
#include "Input/PlayerInputManager.h"
#include "Player/IdleState.h"
#include "Common/Health.h"

namespace Game
{
    void PlayerController::OnStart()
    {
        m_animator = ownerGameObject->GetComponent<Animator>();
        m_rigidbody = ownerGameObject->GetComponent<Rp3dRigidbody>();
        m_collider = ownerGameObject->GetComponent<Rp3dCapsuleCollider>();
        m_health = ownerGameObject->GetComponent<Health>();

        m_rigidbody->SetLinearDamping(PlayerDefaultDamping);

        m_attackState.Initialize(*this);
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
        isGrounded = false;

        // Check player is grounded     
        if (m_rigidbody->GetLinearVelocity().y <= 0.1f)
        {
            bool hasContact = false;
            for (auto& c : m_contacts)
            {
                if (c.normal.y > 0.7f)
                {
                    hasContact = true;
                    break;
                }
            }

            RaycastHit hitInfo;
            Vector3 rayOrigin = ownerGameObject->transform.GetWorldPosition() + m_collider->GetLocalPosition();
            rayOrigin.y -= m_collider->GetHeight() * 0.5f + m_collider->GetRadius() - 0.02f;

            bool isRayHit = ownerGameObject->scene->GetPhysicsSystem()->Raycast(rayOrigin, Vector3(0.0f, -1.0f, 0.0f), 0.1f, hitInfo);

            isGrounded = hasContact || isRayHit;
        }
        m_contacts.clear();

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

    void PlayerController::ChangeState(PlayerState* nextState, bool force)
    {
        if(nextState == nullptr) return;
        if (!force && m_currentState == nextState) return;

        m_currentState->Exit(*this);
        m_currentState = nextState;
        m_currentState->Enter(*this);
    }

    void PlayerController::OnCollisionEnter(CollisionData data)
    {

    }

    void PlayerController::OnCollisionStay(CollisionData data)
    {
        if (data.other->ownerGameObject->GetTag() == ObjectTag::Ground || data.other->ownerGameObject->GetTag() == ObjectTag::Object)
        {
            m_contacts.push_back(data);
        }
    }

    void PlayerController::OnCollisionExit(CollisionData data)
    {

    }

    void PlayerController::OnImGui()
    {
        ImGui::DragFloat("Speed", &moveSpeed, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Dash", &dashSpeed, 1.0f, 0.0f, 1000.0f);
        ImGui::DragFloat("Jump", &jumpImpulse, 1.0f, 0.0f, 1000.0f);
        ImGui::DragFloat("JumpAttack", &jumpAttackImpulse, 1.0f, 0.0f, 1000.0f);
        ImGui::DragInt("AttackPower", &attackPower, 1, 0, 1000);

        ImGui::Checkbox("IsGrounded", &isGrounded);
    }
}