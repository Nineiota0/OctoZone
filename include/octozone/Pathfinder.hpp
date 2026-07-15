#pragma once

#include "octozone/Grid.hpp"
#include "octozone/Path.hpp"
#include "octozone/Position.hpp"

namespace octozone
{

    class Pathfinder
    {
    public:
        static Path findPath(
            const Grid& grid,
            Position start,
            Position goal);

        static Path findPath(
            const Grid& grid,
            Position start,
            Position goal,
            const Path& blockedPositions);
    };

}