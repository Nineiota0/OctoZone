#pragma once

#include "octozone/Grid.hpp"
#include "octozone/Path.hpp"
#include "octozone/Position.hpp"

namespace octozone
{
    class VisionSystem
    {
    public:
        static bool canDetect(
            const Grid& grid,
            const Position& sharkPosition,
            const Position& sharkDirection,
            const Position& octopusPosition,
            int range);

        static Path getVisiblePositions(
            const Grid& grid,
            const Position& sharkPosition,
            const Position& sharkDirection,
            int range);
    };
}