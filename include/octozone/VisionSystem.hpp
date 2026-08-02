#pragma once

#include "octozone/Grid.hpp"
#include "octozone/Path.hpp"
#include "octozone/Position.hpp"

namespace octozone
{
    class VisionSystem
    {
    public:
        // Forward-only line vision. Walls and seaweed block rays. An octopus
        // standing in seaweed is hidden. Direction {0, 0} sees no positions
        // until the shark has a movement direction.
        static bool canDetect(
            const Grid& grid,
            const Position& sharkPosition,
            const Position& sharkDirection,
            const Position& octopusPosition,
            int range);

        // Returns visible positions in the shark's current forward direction,
        // excluding the shark tile and stopping at walls, seaweed, or map edge.
        static Path getVisiblePositions(
            const Grid& grid,
            const Position& sharkPosition,
            const Position& sharkDirection,
            int range);
    };
}
