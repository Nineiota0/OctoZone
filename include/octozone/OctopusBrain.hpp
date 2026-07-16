#pragma once

#include "octozone/GoalPlanner.hpp"
#include "octozone/Grid.hpp"
#include "octozone/HidePlanner.hpp"
#include "octozone/Octopus.hpp"
#include "octozone/Path.hpp"
#include "octozone/Shark.hpp"

#include <optional>

namespace octozone
{
    class OctopusBrain
    {
    public:
        void update(Octopus& octopus, Shark& shark, const Grid& grid);

    private:
        Path addRecentLoopAvoidance(
            const Grid& grid,
            const Octopus& octopus,
            const Path& blockedPositions) const;

        Path getMoveOptions(
            const Grid& grid,
            const Octopus& octopus) const;

        Path findRiskyPathToBestSeaweed(
            const Grid& grid,
            const Octopus& octopus,
            const Path& blockedPositions) const;

        Path findPathToNearestSeaweedFrom(
            const Grid& grid,
            const Octopus& octopus,
            Position start,
            const Path& blockedPositions) const;

        std::optional<Position> chooseTacticalMove(
            const Grid& grid,
            const Octopus& octopus,
            const Shark& shark) const;

        int scoreMove(
            const Grid& grid,
            const Octopus& octopus,
            const Shark& shark,
            Position move) const;

        bool isLooping(const Octopus& octopus) const;
        void moveOneStepWithMemory(Octopus& octopus);

        GoalPlanner goalPlanner_;
        HidePlanner hidePlanner_;

        Path recentPositions_;
    };
}