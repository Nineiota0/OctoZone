#include "octozone/Pathfinder.hpp"

#include <algorithm>
#include <cstdlib>
#include <limits>
#include <optional>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace octozone
{
    namespace
    {
        struct Node
        {
            Position position;
            int gCost{};
            int hCost{};
            std::optional<Position> parent;

            int fCost() const
            {
                return gCost + hCost;
            }
        };

        struct NodeCompare
        {
            bool operator()(const Node& a, const Node& b) const
            {
                return a.fCost() > b.fCost();
            }
        };

        int heuristic(const Position& a, const Position& b)
        {
            return std::abs(a.row - b.row) + std::abs(a.col - b.col);
        }

        std::vector<Position> getNeighbors(const Position& position)
        {
            return {
                {position.row - 1, position.col},
                {position.row + 1, position.col},
                {position.row, position.col - 1},
                {position.row, position.col + 1}
            };
        }

        int key(const Position& position, int cols)
        {
            return position.row * cols + position.col;
        }

        Path reconstructPath(
            Position goal,
            const std::unordered_map<int, Position>& parents,
            int cols)
        {
            Path path;
            Position current = goal;

            path.push_back(current);

            while (parents.contains(key(current, cols)))
            {
                current = parents.at(key(current, cols));
                path.push_back(current);
            }

            std::reverse(path.begin(), path.end());

            // Remove the starting position since the octopus is already there.
            if (!path.empty())
            {
                path.erase(path.begin());
            }

            return path;
        }
    }

    Path Pathfinder::findPath(const Grid& grid, Position start, Position goal)
    {
        std::priority_queue<Node, std::vector<Node>, NodeCompare> openSet;
        std::unordered_set<int> closedSet;
        std::unordered_map<int, int> gCosts;
        std::unordered_map<int, Position> parents;

        openSet.push({start, 0, heuristic(start, goal), std::nullopt});
        gCosts[key(start, grid.getCols())] = 0;

        while (!openSet.empty())
        {
            Node current = openSet.top();
            openSet.pop();

            int currentKey = key(current.position, grid.getCols());

            if (closedSet.contains(currentKey))
            {
                continue;
            }

            if (current.position == goal)
            {
                return reconstructPath(goal, parents, grid.getCols());
            }

            closedSet.insert(currentKey);

            for (Position neighbor : getNeighbors(current.position))
            {
                if (!grid.isInBounds(neighbor))
                {
                    continue;
                }

                if (grid.getTile(neighbor) == Tile::Wall)
                {
                    continue;
                }

                int neighborKey = key(neighbor, grid.getCols());

                if (closedSet.contains(neighborKey))
                {
                    continue;
                }

                int tentativeGCost = current.gCost + 1;

                if (!gCosts.contains(neighborKey) ||
                    tentativeGCost < gCosts[neighborKey])
                {
                    gCosts[neighborKey] = tentativeGCost;
                    parents[neighborKey] = current.position;

                    openSet.push({
                        neighbor,
                        tentativeGCost,
                        heuristic(neighbor, goal),
                        current.position
                    });
                }
            }
        }

        return {};
    }
}