#pragma once

#include <EngineSystem.h>
#include "Player/PlayerState.h"
#include "Player/IdleState.h"
#include "Player/MovingState.h"
#include "Player/JumpingState.h"
#include "Player/WalkingState.h"
#include "Player/RunningState.h"
#include "Player/FallingState.h"
#include "Player/DashState.h"
#include "Player/AttackState.h"
#include "Player/JumpAttackState.h"
#include "Player/DamageState.h"
#include "Player/CombatState.h"
#include "Player/DeathState.h"

namespace Game
{
    using namespace Engine;

    static constexpr float PlayerDefaultDamping = 0.3f;
    static constexpr float CoyoteTime = 0.05f;

    class Health;
    class PlayerController : public Component
    {
    public:
        int killCount = 0;
        int attackPower = 10;
        float moveSpeed = 25.0f;
        float dashSpeed = 100.0f;
        float jumpAttackImpulse = 50.0f;
        float jumpImpulse = 80.0f;
        bool isRunning = false;
        bool isGrounded = false;
        bool hasDamage = false;
        bool isInvincible = false;
        bool isCombat = false;

        void Update(float dt) override;
        void FixedUpdate(float fdt) override;
        
        void OnCollisionEnter(CollisionData data) override;
        void OnCollisionStay(CollisionData data) override;
        void OnCollisionExit(CollisionData data) override;

        // component getters
        Animator* GetAnimator() const { return m_animator; }
        Rp3dRigidbody* GetRigidbody() const { return m_rigidbody; }
        Health* GetHealth() const { return m_health; }
        // state utitlity
        void ChangeState(PlayerState* nextState, bool force = false);
        PlayerState* GetCurrentState() const { return m_currentState; }
        IdleState* GetIdleState() { return &m_idleState; }
        WalkingState* GetWalkingState() { return &m_walkingState; }
        RunningState* GetRunningState() { return &m_runningState; }
        JumpingState* GetJumpingState() { return &m_jumpingState; }
        FallingState* GetFallingState() { return &m_fallingState; }
        DashState* GetDashState() { return &m_dashState; }
        AttackState* GetAttackState() { return &m_attackState; }
        JumpAttackState* GetJumpAttackState() { return &m_jumpAttackState; }
        DamageState* GetDamageState() { return &m_damageState; }
        CombatState* GetCombatState() { return &m_combatState; }
        DeathState* GetDeathState() { return &m_deathState; }

        // jump utitlity
        bool CanJump() const { return isGrounded || m_coyoteTimer > 0.0f; }
        float GetCoyoteTime() const { return m_coyoteTimer; }
        void ConsumeJump() { m_coyoteTimer = 0.0f; }

        void OnImGui() override;

    protected:
        void OnStart() override;

    private:
        Animator* m_animator = nullptr;
        Rp3dRigidbody* m_rigidbody = nullptr;
        Rp3dCapsuleCollider* m_collider = nullptr;
        Health* m_health = nullptr;

        PlayerState* m_currentState = nullptr;
        IdleState m_idleState;
        WalkingState m_walkingState;
        RunningState m_runningState;
        JumpingState m_jumpingState;
        FallingState m_fallingState;
        DashState m_dashState;
        AttackState m_attackState;
        JumpAttackState m_jumpAttackState;
        DamageState m_damageState;
        CombatState m_combatState;
        DeathState m_deathState;

        std::vector<CollisionData> m_contacts;
        float m_coyoteTimer = 0.0f; // Timer allow jumping in grounded false
    };
}