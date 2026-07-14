#include "octozone/VisionSystem.hpp"

#include <cstdlib>
#include <vector>

namespace octozone
{
    namespace
    {
        bool isBlocked(
            const Grid& grid,
            const Position& sharkPosition,
            const Position& targetPosition)
        {
            int rowStep = 0;
            int colStep = 0;

            if (targetPosition.row > sharkPosition.row)
            {
                rowStep = 1;
            }
            else if (targetPosition.row < sharkPosition.row)
            {
                rowStep = -1;
            }

            if (targetPosition.col > sharkPosition.col)
            {
                colStep = 1;
            }
            else if (targetPosition.col < sharkPosition.col)
            {
                colStep = -1;
            }

            Position current{
                sharkPosition.row + rowStep,
                sharkPosition.col + colStep
            };

            while (current != targetPosition)
            {
                if (!grid.isInBounds(current))
                {
                    return true;
                }

                if (grid.getTile(current) == Tile::Wall)
                {
                    return true;
                }

                current.row += rowStep;
                current.col += colStep;
            }

            return false;
        }
    }

    bool VisionSystem::canDetect(
        const Grid& grid,
        const Position& sharkPosition,
        const Position& sharkDirection,
        const Position& octopusPosition,
        int range)
    {
        if (sharkDirection == Position{0, 0})
        {
            return false;
        }

        std::vector<Position> directions{
            sharkDirection
        };

        if (sharkDirection.row == 0)
        {
            directions.push_back({-1, sharkDirection.col});
            directions.push_back({1, sharkDirection.col});
        }
        else
        {
            directions.push_back({sharkDirection.row, -1});
            directions.push_back({sharkDirection.row, 1});
        }

        for (const Position& direction : directions)
        {
            for (int distance = 1; distance <= range; ++distance)
            {
                Position visiblePosition{
                    sharkPosition.row + direction.row * distance,
                    sharkPosition.col + direction.col * distance
                };

                if (!grid.isInBounds(visiblePosition))
                {
                    break;
                }

                if (grid.getTile(visiblePosition) == Tile::Wall)
                {
                    break;
                }

                if (visiblePosition == octopusPosition &&
                    !isBlocked(grid, sharkPosition, octopusPosition))
                {
                    return true;
                }
            }
        }

        return false;
    }
}