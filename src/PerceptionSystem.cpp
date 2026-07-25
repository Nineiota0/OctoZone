#include "octozone/PerceptionSystem.hpp"

#include <cstdlib>

namespace octozone
{
    bool PerceptionSystem::canSeeShark(
        const Grid& grid,
        Position octopusPosition,
        Position sharkPosition,
        int range)
    {
        int rowDelta = sharkPosition.row - octopusPosition.row;
        int colDelta = sharkPosition.col - octopusPosition.col;
        int distance = std::abs(rowDelta) + std::abs(colDelta);

        if (canSenseNearbyShark(octopusPosition, sharkPosition))
        {
            return true;
        }

        if (distance > range)
        {
            return false;
        }

        if (distance <= 1)
        {
            return true;
        }

        if (rowDelta != 0 && colDelta != 0)
        {
            return false;
        }

        Position step{
            rowDelta == 0 ? 0 : rowDelta / std::abs(rowDelta),
            colDelta == 0 ? 0 : colDelta / std::abs(colDelta)
        };

        Position current{
            octopusPosition.row + step.row,
            octopusPosition.col + step.col
        };

        while (current != sharkPosition)
        {
            if (!grid.isInBounds(current) ||
                grid.getTile(current) == Tile::Wall ||
                grid.getTile(current) == Tile::Seaweed)
            {
                return false;
            }

            current = {
                current.row + step.row,
                current.col + step.col
            };
        }

        return true;
    }

    bool PerceptionSystem::canSenseNearbyShark(
        Position octopusPosition,
        Position sharkPosition,
        int range)
    {
        return std::abs(sharkPosition.row - octopusPosition.row) +
            std::abs(sharkPosition.col - octopusPosition.col) <= range;
    }
}
