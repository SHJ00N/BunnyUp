#pragma once

#include <EngineSystem.h>
#include <queue>
#include "NavigationGrid.h"

namespace Game
{
    struct AStarNode
    {
        int gCost = INT_MAX;
        int hCost = 0;

        bool closed = false;
        int parentIndex = -1;

        int FCost() const { return gCost + hCost; }
    };

    struct OpenedNode
    {
        int index;
        int fCost = 0;
    };

    struct OpendNodeCompare
    {

        bool operator()(const OpenedNode& a, const OpenedNode& b)
        {
            return a.fCost > b.fCost;
        }
    };

    class AStar
    {
    public:
        std::vector<Engine::Vector3> FindPath(const NavigationGrid& grid, const Engine::Vector3& start, const std::vector<GridCoord>& goalCells);

    private:
        std::vector<AStarNode> m_nodes;
        int m_nodesWidth = 0;
        int m_nodesHeight = 0;

        bool isValid(int x, int y);
        void updateNeighbors(const NavigationGrid& grid, int currentIndex, const std::vector<GridCoord>& goalCells, std::priority_queue<OpenedNode, std::vector<OpenedNode>, OpendNodeCompare>& openedList);
        int calculateHCost(int x, int y, const std::vector<GridCoord>& goalCells);
    };
}