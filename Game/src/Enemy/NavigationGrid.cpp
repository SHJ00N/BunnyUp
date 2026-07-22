#include "Enemy/NavigationGrid.h"

namespace Game
{
    using namespace Engine;

    bool NavigationGrid::Build(Engine::Scene* scene, float cellSize)
    {
        m_cells.clear();
        m_cellSize = cellSize;

        // Create bounds about all bound
        AABB mapBounds;
        bool first = true;
        for (auto* collider : scene->GetObstacleColliders())
        {
            if (first)
            {
                mapBounds = collider->GetBounds();
                first = false;
            }
            else
            {
                mapBounds.MergeBounds(collider->GetBounds());
            }
        }

        // collider is empty
        if (first)
        {
            return false;
        }

        // Calculate grid size
        Vector3 min = mapBounds.GetMin();
        Vector3 max = mapBounds.GetMax();

        m_origin = min;
        m_width = static_cast<int>(std::ceil((max.x - min.x) / m_cellSize));
        m_height = static_cast<int>(std::ceil((max.z - min.z) / m_cellSize));

        float cellHeight = max.y - min.y;
        float cellCenterY = (min.y + max.y) * 0.5f;

        // Create cells
        m_cells.resize(m_width * m_height);
        for (int y = 0; y < m_height; ++y)
        {
            for (int x = 0; x < m_width; ++x)
            {
                NaviGridCell& cell = m_cells[y * m_width + x];
                cell.walkable = true;
                cell.coord.x = x;
                cell.coord.y = y;
                cell.bounds = AABB(
                    Vector3(m_origin.x + (x + 0.5f) * m_cellSize, cellCenterY, m_origin.z + (y + 0.5f) * m_cellSize), // center position
                    m_cellSize * 0.5f,  // x extent
                    cellHeight * 0.5f,   // y extent
                    m_cellSize * 0.5f   // z extent
                );
            }
        }

        // Check cell is walkable
        for (auto* collider : scene->GetObstacleColliders())
        {
            AABB bounds = collider->GetBounds();
            // Get range of coordinates of collider on navigationGrid
            Vector3 min = bounds.GetMin();
            Vector3 max = bounds.GetMax();
            
            int minX = static_cast<int>(std::floor((min.x - m_origin.x) / m_cellSize));
            int maxX = static_cast<int>(std::floor((max.x - m_origin.x) / m_cellSize));

            int minY = static_cast<int>(std::floor((min.z - m_origin.z) / m_cellSize));
            int maxY = static_cast<int>(std::floor((max.z - m_origin.z) / m_cellSize));

            // Clamp for indexing
            minX = std::max(0, minX);
            maxX = std::min(m_width - 1, maxX);

            minY = std::max(0, minY);
            maxY = std::min(m_height - 1, maxY);

            for (int y = minY; y <= maxY; ++y)
            {
                for (int x = minX; x <= maxX; ++x)
                {
                    auto& cell = m_cells[y * m_width + x];
                    if (cell.bounds.IsIntersectsAABB(bounds))
                    {
                        cell.walkable = false;
                    }
                }
            }
        }

        return true;
    }

    const NaviGridCell& NavigationGrid::GetCell(int x, int y) const
    {
        assert(x >= 0 && x < m_width && "Out of Range on navigationgrid");
        assert(y >= 0 && y < m_height && "Out of Range on navigationgrid");
        return m_cells[y * m_width + x];
    }

    const NaviGridCell& NavigationGrid::GetCell(int index) const
    {
        assert(index < m_cells.size() && "Out of Range on navigationgrid");
        return m_cells[index];
    }

    GridCoord NavigationGrid::WorldToCell(const Engine::Vector3& worldPos) const
    {
        int x = static_cast<int>(std::floor((worldPos.x - m_origin.x) / m_cellSize));
        int y = static_cast<int>(std::floor((worldPos.z - m_origin.z) / m_cellSize));

        // Clamp
        x = std::clamp(x, 0, m_width - 1);
        y = std::clamp(y, 0, m_height - 1);

        return { x, y };
    }

    Vector3 NavigationGrid::CellToWorld(int index) const
    {
        assert(index < m_cells.size() && "Out of Range on navigationgrid");
        return m_cells[index].bounds.center;
    }

    std::vector<GridCoord> NavigationGrid::GetContactCoordsForBounds(const AABB& bounds)
    {
        std::vector<GridCoord> result;

        Vector3 min = bounds.GetMin();
        Vector3 max = bounds.GetMax();

        int minX = static_cast<int>(std::floor((min.x - m_origin.x) / m_cellSize));
        int maxX = static_cast<int>(std::floor((max.x - m_origin.x) / m_cellSize));

        int minY = static_cast<int>(std::floor((min.z - m_origin.z) / m_cellSize));
        int maxY = static_cast<int>(std::floor((max.z - m_origin.z) / m_cellSize));

        // Clamp for indexing
        minX = std::max(0, minX);
        maxX = std::min(m_width - 1, maxX);

        minY = std::max(0, minY);
        maxY = std::min(m_height - 1, maxY);

        for (int y = minY; y <= maxY; ++y)
        {
            for (int x = minX; x <= maxX; ++x)
            {
                auto& cell = m_cells[y * m_width + x];
                if (cell.bounds.IsIntersectsAABB(bounds) && cell.walkable)
                {
                    result.push_back({ x, y });
                }
            }
        }

        return result;
    }
}