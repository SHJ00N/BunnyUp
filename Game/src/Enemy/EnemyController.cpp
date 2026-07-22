#include "Enemy/EnemyController.h"
#include "Common/Health.h"

namespace Game
{
    void EnemyController::OnStart()
    {
        m_rigidbody = ownerGameObject->GetComponent<Engine::Rp3dRigidbody>();
        m_rigidbody->SetLinearDamping(1.0f);
        m_animator = ownerGameObject->GetComponent<Engine::Animator>();
        m_health = ownerGameObject->GetComponent<Health>();

        m_attack.Initialize(*this);
    }

    void EnemyController::Update(float dt)
    {
        // update timer 
        m_attack.UpdateTimer(dt);
        m_movement.UpdatePath(*this, m_aStar);

        if (!m_currentState)
        {
            return;
        }

        m_currentState->Update(*this, dt);

        // debug draw
        if (debugDraw)
        {
            m_movement.DebugDraw(*this);
            m_attack.DebugDraw(*this);
        }
    }

    void EnemyController::ChangeState(EnemyState* nextState, bool force)
    {
        if (nextState == nullptr) return;
        if (!force && m_currentState == nextState) return;

        if (m_currentState)
        {
            m_currentState->Exit(*this);
        }
        m_currentState = nextState;
        m_currentState->Enter(*this);
    }


    void EnemyController::OnImGui()
    {
        ImGui::Checkbox("Debug Draw", &debugDraw);
        ImGui::DragInt("AttackPower", &attackPower, 1, 0, 1000);
        m_movement.EditorGui();
        m_attack.EditorGui();
    }
}