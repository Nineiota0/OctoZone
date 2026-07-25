#pragma once

#include "octozone/Grid.hpp"
#include "octozone/Position.hpp"

namespace octozone
{
    class PerceptionSystem
    {
    public:
        static bool canSeeShark(
            const Grid& grid,
            Position octopusPosition,
            Position sharkPosition,
            int range = 7);

        static bool canSenseNearbyShark(
            Position octopusPosition,
            Position sharkPosition,
            int range = 2);
    };
}
