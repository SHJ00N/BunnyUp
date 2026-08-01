#pragma once

#include <EngineSystem.h>
#include "Singleton.h"

namespace Game
{
    class EnemySpawnManager;
    class PlayerController;
    class GameManager : public Singleton<GameManager>
    {
    public:
        PlayerController* playerController = nullptr;
        Engine::TextComponent* healthText = nullptr;
        Engine::TextComponent* killCountText = nullptr;
        Engine::TextComponent* gameOverText = nullptr;
        Engine::TextComponent* restartText = nullptr;
        Engine::TextComponent* gameClearText = nullptr;

        void Update(float dt) override;

    private:
        bool m_isGameOver = false;
        bool m_isGameClear = false;
        bool m_isGameEnd = false;

        float m_directingTimer = 0.0f;
    };
}