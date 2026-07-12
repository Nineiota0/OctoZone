#include "octozone/VisionSystem.hpp"

#include <cstdlib>

namespace octozone
{
    bool VisionSystem::canDetect(
        const Grid& grid,
        const Position& sharkPosition,
        const Position& octopusPosition,
        int range)
    {
        bool sameRow = sharkPosition.row == octopusPosition.row;
        bool sameCol = sharkPosition.col == octopusPosition.col;

        if (!sameRow && !sameCol)
        {
            return false;
        }

        int distance =
            std::abs(sharkPosition.row - octopusPosition.row) +
            std::abs(sharkPosition.col - octopusPosition.col);

        if (distance > range)
        {
            return false;
        }

        int rowStep = 0;
        int colStep = 0;

        if (sameRow)
        {
            colStep = (octopusPosition.col > sharkPosition.col) ? 1 : -1;
        }
        else
        {
            rowStep = (octopusPosition.row > sharkPosition.row) ? 1 : -1;
        }

        Position current{
            sharkPosition.row + rowStep,
            sharkPosition.col + colStep
        };

        while (current != octopusPosition)
        {
            if (grid.getTile(current) == Tile::Wall)
            {
                return false;
            }

            current.row += rowStep;
            current.col += colStep;
        }

        return true;
    }
}