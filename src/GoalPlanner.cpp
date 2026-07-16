#include "octozone/GoalPlanner.hpp"
#include "octozone/Pathfinder.hpp"

namespace octozone
{
    Path GoalPlanner::findPathToGoal(
        const Grid& grid,
        const Octopus& octopus,
        const Path& danger) const
    {
        return Pathfinder::findPath(
            grid,
            octopus.getPosition(),
            octopus.getGoal(),
            danger
        );
    }
}