#include "octozone/HidePlanner.hpp"
#include "octozone/DangerSystem.hpp"
#include "octozone/Pathfinder.hpp"

#include <cstdlib>

namespace octozone
{
    Path HidePlanner::findPathToBestSeaweed(
        const Grid& grid,
        const Octopus& octopus,
        const std::vector<Shark>& sharks,
        const Path& danger) const
    {
        Path bestPath;

        for (int row = 0; row < grid.getRows(); ++row)
        {
            for (int col = 0; col < grid.getCols(); ++col)
            {
                Position position{row, col};

                if (grid.getTile(position) != Tile::Seaweed ||
                    !DangerSystem::isSafePosition(grid, sharks, position, danger))
                {
                    continue;
                }

                Path path = Pathfinder::findPath(
                    grid,
                    octopus.getPosition(),
                    position,
                    danger
                );

                if (!path.empty() &&
                    (bestPath.empty() ||
                     path.size() < bestPath.size() ||
                     (path.size() == bestPath.size() &&
                      manhattanDistance(position, octopus.getGoal()) <
                      manhattanDistance(bestPath.back(), octopus.getGoal()))))
                {
                    bestPath = path;
                }
            }
        }

        return bestPath;
    }

    int HidePlanner::manhattanDistance(
        const Position& a,
        const Position& b) const
    {
        return std::abs(a.row - b.row) + std::abs(a.col - b.col);
    }
}
