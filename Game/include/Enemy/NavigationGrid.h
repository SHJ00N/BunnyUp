#pragma once

#include <EngineSystem.h>

namespace Game
{
    struct GridCoord
    {
        int x;
        int y;
    };

    struct NaviGridCell
    {
        bool walkable = true;   // state of walkable on this cell
        GridCoord coord;   // coordinate of the cell
        Engine::AABB bounds;  // aabb bounds of  the cell
    };

    class NavigationGrid
    {
    public:
        bool Build(Engine::Scene* scene, float cellSize);

        float GetCellSize() const { return m_cellSize; }
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }
        const NaviGridCell& GetCell(int x, int y) const;
        const NaviGridCell& GetCell(int index) const;
        const std::vector<NaviGridCell>& GetCells() const { return m_cells; }

        GridCoord WorldToCell(const Engine::Vector3& worldPos) const;
        Engine::Vector3 CellToWorld(int index) const;

        std::vector<GridCoord> GetContactCoordsForBounds(const Engine::AABB& bounds);

    private:
        float m_cellSize;

        int m_width;
        int m_height;

        Engine::Vector3 m_origin;   // center position of grid

        std::vector<NaviGridCell> m_cells;
    };
}