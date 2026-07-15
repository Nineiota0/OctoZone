#include "octozone/VisionSystem.hpp"

#include <algorithm>
#include <vector>

namespace octozone
{
    Path VisionSystem::getVisiblePositions(
        const Grid& grid,
        const Position& sharkPosition,
        const Position& sharkDirection,
        int range)
    {
        Path visiblePositions;

        if (sharkDirection == Position{0, 0})
        {
            return visiblePositions;
        }

        std::vector<Position> directions{
            sharkDirection
        };

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

                if (grid.getTile(visiblePosition) == Tile::Wall ||
                    grid.getTile(visiblePosition) == Tile::Seaweed)
                {
                    break;
                }

                visiblePositions.push_back(visiblePosition);
            }
        }

        return visiblePositions;
    }

    bool VisionSystem::canDetect(
        const Grid& grid,
        const Position& sharkPosition,
        const Position& sharkDirection,
        const Position& octopusPosition,
        int range)
    {
        if (grid.getTile(octopusPosition) == Tile::Seaweed)
        {
            return false;
        }
    
        Path visiblePositions = getVisiblePositions(
            grid,
            sharkPosition,
            sharkDirection,
            range);
        
        return std::find(
            visiblePositions.begin(),
            visiblePositions.end(),
            octopusPosition
        ) != visiblePositions.end();
    }
}