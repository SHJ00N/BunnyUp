#pragma once

#include <EngineSystem.h>
#include "Enemy/AStar.h"
#include "Enemy/EnemyState.h"
#include "Enemy/EnemyMovement.h"
#include "Enemy/EnemyIdle.h"
#include "Enemy/EnemyDamaged.h"
#include "Enemy/EnemyDeath.h"

namespace Game
{
    class Health;
    class EnemyController : public Engine::Component
    {
    public:
        int attackPower = 10;
        bool debugDraw = false;

        // override function
        void Update(float dt) override;
        void OnImGui() override;

        // getter and setter
        Engine::Rp3dRigidbody* GetRigidbody() const { return m_rigidbody; }
        Engine::Animator* GetAnimator() const { return m_animator; }
        Health* GetHealth() const { return m_health; }
        
        // state management
        void ChangeState(EnemyState* nextState, bool force = false);
        EnemyState* GetCurrentState() const { return m_currentState; }
        EnemyIdle* GetIdle() { return&m_idle; }
        EnemyMovement* GetMovement() { return &m_movement; }
        EnemyDamaged* GetDamaged() { return &m_damaged; }
        EnemyDeath* GetDeath() { return &m_death; }
        
        Engine::GameObject* GetTarget() const { return m_target; }
        void SetTarget(Engine::GameObject* target) { m_target = target; }
        

    protected:
        virtual void OnStart() override;

        virtual void OnUpdate(float dt) { }
        virtual void OnDebug() { }
        virtual void OnGui() { }

    private:
         AStar m_aStar;  // calculate path to target position

        Engine::GameObject* m_target = nullptr;

        Engine::Rp3dRigidbody* m_rigidbody = nullptr;
        Engine::Animator* m_animator = nullptr;
        Health* m_health = nullptr;
        
        EnemyState* m_currentState = nullptr;
        EnemyIdle m_idle;
        EnemyMovement m_movement;
        EnemyDamaged m_damaged;
        EnemyDeath m_death;
    };
}