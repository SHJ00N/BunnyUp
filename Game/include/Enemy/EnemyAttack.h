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
        void Initialize(EnemyController& controller);

        // override function
        void Enter(EnemyController& controller) override;
        void Exit(EnemyController& controller) override;
        
        void UpdateTimer(float dt);
        bool IsTargetInRange(EnemyController& controller) const;
        bool IsAttacking() const;
        bool CanAttack() const;

        void EditorGui();
        void DebugDraw(EnemyController& controller) const;

    protected:
        void OnUpdate(EnemyController& controller, float dt) override;

    private:
        EnemyAttackHitBox* m_hitBox = nullptr;

        float m_attackRange = 15.0f;

        float m_attackCoolTime = 2.5f;
        float m_attackCoolTimer = 0.0f;

        bool m_isAttacked = false;
        float m_attackTimer = 0.0f;

        bool m_isAttacking = false;
    };
}