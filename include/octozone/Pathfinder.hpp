#pragma once

#include "octozone/Grid.hpp"
#include "octozone/Path.hpp"
#include "octozone/Position.hpp"

namespace octozone
{

    class Pathfinder
    {
    public:
        // A* over four-way grid movement. Returned paths exclude the start tile.
        // Walls are always blocked, unreachable targets return an empty path, and
        // start == goal also returns an empty path because no movement is needed.
        static Path findPath(
            const Grid& grid,
            Position start,
            Position goal);

        // blockedPositions are treated as temporary danger/collision blockers.
        // Seaweed may still be entered even when listed as blocked so hiding
        // remains possible while escaping.
        static Path findPath(
            const Grid& grid,
            Position start,
            Position goal,
            const Path& blockedPositions);
    };

}
