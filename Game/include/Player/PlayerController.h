#pragma once

#include <EngineSystem.h>
#include "Player/PlayerState.h"
#include "Player/IdleState.h"
#include "Player/MovingState.h"
#include "Player/JumpingState.h"
#include "Player/WalkingState.h"
#include "Player/RunningState.h"
#include "Player/FallingState.h"

namespace Game
{
    using namespace Engine;

    static constexpr float GroundLostDelay = 0.05f;
    static constexpr float CoyoteTime = 1.0f;

    class PlayerFoot;
    class PlayerController : public Component
    {
    public:
        float moveSpeed = 10.0f;
        bool isGrounded = false;
        bool isFootGrounded = false;

        void Update(float dt) override;
        void FixedUpdate(float fdt) override;
        void OnCollisionEnter(Rp3dCollider* other) override;
        void OnCollisionExit(Rp3dCollider* other) override;

        // component getters
        Animator* GetAnimator() const { return m_animator; }
        Rp3dRigidbody* GetRigidbody() const { return m_rigidbody; }
        // state utitlity
        void ChangeState(PlayerState* nextState);
        PlayerState* GetCurrentState() const { return m_currentState; }
        IdleState* GetIdleState() { return &m_idleState; }
        WalkingState* GetWalkingState() { return &m_walkingState; }
        RunningState* GetRunningState() { return &m_runningState; }
        JumpingState* GetJumpingState() { return &m_jumpingState; }
        FallingState* GetFallingState() { return &m_fallingState; }

        // jump utitlity
        bool CanJump() const { return isGrounded || m_coyoteTimer > 0.0f; }
        void ConsumeJump() { m_coyoteTimer = 0.0f; }

    protected:
        void OnStart() override;

    private:
        Animator* m_animator = nullptr;
        Rp3dRigidbody* m_rigidbody = nullptr;
        Rp3dCapsuleCollider* m_collider = nullptr;

        PlayerState* m_currentState = nullptr;
        IdleState m_idleState;
        WalkingState m_walkingState;
        RunningState m_runningState;
        JumpingState m_jumpingState;
        FallingState m_fallingState;

        float m_groundLostTimer = 0.05f; // Time to consider the player grounded after leaving the ground
        float m_coyoteTimer = 0.0f; // Timer allow jumping in grounded false
    };
}