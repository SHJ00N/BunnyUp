#pragma once

#include "Enemy/EnemyAttack.h"

namespace Game
{
    class EnemyController;
    class EnemyAttackHitBox;

    class MeleeEnemyAttack : public EnemyAttack
    {
    public:
        void SetHitBoxCenter(const Engine::Vector3& center) { m_hitBoxCenter = center; }
        void SetHitBoxSize(const Engine::Vector3& size) { m_hitBoxSize = size; }
        EnemyAttackHitBox* CreateHitBox(EnemyController& controller);

    protected:
        void OnReset() override;
        void EnterAttack(EnemyController& controller) override;
        void ExitAttack(EnemyController& controller) override;
        void StartAttack(EnemyController& controller) override;
        void EndAttack(EnemyController& controller) override;

    private:
        EnemyAttackHitBox* m_hitBox = nullptr;

        Engine::Vector3 m_hitBoxCenter;
        Engine::Vector3 m_hitBoxSize;
    };
}