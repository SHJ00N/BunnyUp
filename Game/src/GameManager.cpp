#include "GameManager.h"
#include "Player/PlayerController.h"
#include "Common/Health.h"
#include "Input/PlayerInputManager.h"
#include "Enemy/EnemySpawnManager.h"

namespace Game
{
    using namespace Engine;
    void GameManager::Update(float dt)
    {
        if (playerController && healthText)
        {
            healthText->SetText("Health: " + std::to_string(playerController->GetHealth()->GetCurrentHealth()));
        }

        if(playerController && killCountText)
        {
            killCountText->SetText("Kill: " + std::to_string(playerController->killCount));
        }

        if (m_isGameEnd && PlayerInputManager::GetInstance().GetInputState().start)
        {
            m_isGameEnd = false;
            m_isGameOver = false;
            m_isGameClear = false;
            m_directingTimer = 0.0f;
            // Reset the scene
            TimeClass::SetTimeScale(1.0f);
            SceneManager::GetInstance().LoadScene("GameScene");
            return;
        }

        if (m_isGameEnd)
        {
            return;
        }

        if(m_isGameClear)
        {
            m_directingTimer += dt;
            if(m_directingTimer >= 1.0f)
            {
                TimeClass::SetTimeScale(1.0f);
                m_directingTimer = 0.0f;

                restartText->ownerGameObject->Enable();
                m_isGameEnd = true;
            }
        }

        if (m_isGameOver)
        {
            m_directingTimer += dt;
            if(m_directingTimer >= 3.0f)
            {
                m_directingTimer = 0.0f;

                restartText->ownerGameObject->Enable();
                m_isGameEnd = true;
            }
        }

        if(playerController && playerController->GetHealth()->IsDeath() && !m_isGameOver)
        {
            m_isGameOver = true;
            gameOverText->ownerGameObject->Enable();
        }

        if(playerController && playerController->killCount >= EnemySpawnManager::GetInstance().GetMaxSpawnCount() && !m_isGameClear)
        {
            m_isGameClear = true;
            gameClearText->ownerGameObject->Enable();
            TimeClass::SetTimeScale(0.1f);
        }
    }
}