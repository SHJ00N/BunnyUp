#pragma once

#include <EngineSystem.h>
#include "Enemy/EnemyState.h"

namespace Game
{
    class EnemyController;
    class EnemyAttackHitBox;

    class EnemyAttack : public EnemyState
    {
    public:
        virtual void Initialize(EnemyController& controller) { }
        void Reset();

        // override function
        void Enter(EnemyController& controller) override;
        void Exit(EnemyController& controller) override;

        void SetAttackCoolTime(float coolTime) { m_attackCoolTime = coolTime; }
        void SetAttackRange(float range) { m_attackRange = range; }
        
        void UpdateTimer(float dt);
        bool IsTargetInRange(EnemyController& controller) const;
        bool IsAttacking() const;
        bool CanAttack() const;

        void EditorGui();
        void DebugDraw(EnemyController& controller) const;

    protected:
        void OnUpdate(EnemyController& controller, float dt) override;
        
        virtual void OnReset() { }
        virtual void EnterAttack(EnemyController& controller) { }
        virtual void ExitAttack(EnemyController& controller) { }
        virtual void StartAttack(EnemyController& controller) { }
        virtual void EndAttack(EnemyController& controller) { }

    private:
        float m_attackRange = 15.0f;

        float m_attackCoolTime = 2.5f;
        float m_attackCoolTimer = 0.0f;

        bool m_isAttacked = false;
        float m_attackTimer = 0.0f;

        bool m_isAttacking = false;
    };
}