#pragma once

#include "octozone/Grid.hpp"
#include "octozone/Octopus.hpp"
#include "octozone/Path.hpp"

namespace octozone
{
    class GoalPlanner
    {
    public:
        Path findPathToGoal(
            const Grid& grid,
            const Octopus& octopus,
            const Path& danger) const;
    };
}