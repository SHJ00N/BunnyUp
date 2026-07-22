#include <Enemy/AStar.h>

namespace Game
{
    using namespace Engine;

    std::vector<Vector3> AStar::FindPath(const NavigationGrid& grid, const Vector3& start, const std::vector<GridCoord>& goalCells)
    {
        std::vector<Vector3> path;
        
        if (goalCells.empty())
        {
            return path;
        }

        // create aStar nodes from navigation grid
        const int width = grid.GetWidth();
        const int height = grid.GetHeight();

        m_nodesWidth = width;
        m_nodesHeight = height;
        
        const int nodeCount = width * height;
        if (m_nodes.size() != nodeCount)
        {
            m_nodes.resize(nodeCount);
        }

        // Initialize nodes
        for (auto& node : m_nodes)
        {
            node.gCost = INT_MAX;
            node.hCost = 0;
            node.closed = false;
            node.parentIndex = -1;
        }

        auto startCoord = grid.WorldToCell(start);
        const int startIndex = startCoord.y * width + startCoord.x;
        
        // process A*
        m_nodes[startIndex].gCost = 0;
        m_nodes[startIndex].hCost = calculateHCost(startCoord.x, startCoord.y, goalCells);
        std::priority_queue<OpenedNode, std::vector<OpenedNode>, OpendNodeCompare> openedList;
        openedList.push({ startIndex, m_nodes[startIndex].FCost()});
        int goalIndex = -1;
        while (!openedList.empty())
        {
            auto current = openedList.top();
            openedList.pop();

            int currentIndex = current.index;
            // check closed
            if (m_nodes[currentIndex].closed)
            {
                continue;
            }
            // check fcost missmatch
            if (current.fCost != m_nodes[currentIndex].FCost())
            {
                continue;
            }   

            m_nodes[currentIndex].closed = true;

            // check arrived
            for (const auto& cell : goalCells)
            {
                if (cell.x == grid.GetCell(currentIndex).coord.x && cell.y == grid.GetCell(currentIndex).coord.y)
                {
                    goalIndex = currentIndex;
                    break;
                }
            }
            if (goalIndex != -1) break;

            updateNeighbors(grid, currentIndex, goalCells, openedList);
        }

        if (goalIndex != -1)
        {
            int currentIndex = goalIndex;

            while (currentIndex != -1)
            {
                path.push_back(grid.CellToWorld(currentIndex));
                currentIndex = m_nodes[currentIndex].parentIndex;
            }

            std::reverse(path.begin(), path.end());
        }

        return path;
    }

    bool AStar::isValid(int x, int y)
    {
        return x >= 0 && y >= 0 && x < m_nodesWidth && y < m_nodesHeight;
    }

    void AStar::updateNeighbors(const NavigationGrid& grid, int currentIndex, const std::vector<GridCoord>& goalCells, std::priority_queue<OpenedNode, std::vector<OpenedNode>, OpendNodeCompare>& openedList)
    {
        static constexpr int offset[8][4] =
        {
            // x, y, cost, diagonal flag
            {-1,  0, 10, false},
            { 1,  0, 10, false},
            { 0, -1, 10, false},
            { 0,  1, 10, false},
            {-1, -1, 14, true},
            {-1,  1, 14, true},
            { 1, -1, 14, true},
            { 1,  1, 14, true},
        };
    
        const auto& currentCell = grid.GetCell(currentIndex);
        for (int i = 0; i < 8; ++i)
        {
            int offsetX = offset[i][0];
            int offsetY = offset[i][1];
            int cost = offset[i][2];
            bool isDiagonal = offset[i][3];

            int currentX = currentCell.coord.x + offsetX;
            int currentY = currentCell.coord.y + offsetY;

            // check valid node
            if (!isValid(currentX, currentY)) continue;
            
            int neighborIndex = currentY * m_nodesWidth + currentX;
            if (m_nodes[neighborIndex].closed || !grid.GetCell(neighborIndex).walkable)
            {
                continue;
            }

            if (isDiagonal)
            {
                if (!isValid(currentX, currentCell.coord.y) || !isValid(currentCell.coord.x, currentY))
                {
                    continue;
                }

                if (!grid.GetCell(currentX, currentCell.coord.y).walkable || !grid.GetCell(currentCell.coord.x, currentY).walkable)
                {
                    continue;
                }
            }

            // node update process
            auto& node = m_nodes[neighborIndex];
            
            int newCost = m_nodes[currentIndex].gCost + cost;
            if (newCost < node.gCost)
            {
                node.gCost = newCost;
                node.parentIndex = currentIndex;
                node.hCost = calculateHCost(currentX, currentY, goalCells);

                openedList.push({ neighborIndex, node.FCost() });
            }
        }
    }

    int AStar::calculateHCost(int x, int y, const std::vector<GridCoord>& goalCells)
    {
        int minCost = INT_MAX;

        for (const auto& goal : goalCells)
        {
            // calculate Octile distance
            int dx = std::abs(goal.x - x);
            int dy = std::abs(goal.y - y);

            int h = 14 * std::min(dx, dy) + 10 * std::abs(dx - dy);

            minCost = std::min(minCost, h);
        }
        return minCost;
    }

}