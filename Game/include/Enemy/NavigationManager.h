#pragma once

#include <EngineSystem.h>
#include "Singleton.h"
#include "Enemy/NavigationGrid.h"

namespace Game
{
    class NavigationManager : public Singleton<NavigationManager>
    {
    public:
        NavigationManager(float m_cellSize = 1.0f);

        void Update(float dt) override;
        void OnImGui() override;

        const NavigationGrid& GetGrid() const { return m_grid; }
        NavigationGrid& GetGrid() { return m_grid; }

    private:
        NavigationGrid m_grid;
        Engine::Scene* m_currentScene = nullptr;
        float m_cellSize;
        bool m_debugDraw = false;
    };
}