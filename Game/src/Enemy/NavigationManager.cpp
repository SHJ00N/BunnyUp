#include "Enemy/NavigationManager.h"

namespace Game
{
    using namespace Engine;

    NavigationManager::NavigationManager(float cellSize) : m_cellSize(cellSize)
    {
    }

    void NavigationManager::Update(float dt)
    {
        Scene* currentScene = ownerGameObject->scene;

        if (currentScene != m_currentScene)
        {
            m_currentScene = currentScene;

            if (m_currentScene)
            {
                m_grid.Build(m_currentScene, m_cellSize);
            }
        }

        if (currentScene && m_debugDraw)
        {
            for (const auto& cell : m_grid.GetCells())
            {
                Vector4 color = cell.walkable ? Vector4(0.0f, 1.0f, 0.0f, 1.0f) : Vector4(1.0f, 0.0f, 0.0f, 1.0f);
                DebugRenderer::GetInstance().AddAABB(&cell.bounds, color);
            }
        }
    }

    void NavigationManager::OnImGui()
    {
        ImGui::Checkbox("Visible", &m_debugDraw);

        if (ImGui::DragFloat("CellSize", &m_cellSize, 0.1f, 1.0f, 100.0f))
        {
            m_grid.Build(m_currentScene, m_cellSize);
        }
    }
}