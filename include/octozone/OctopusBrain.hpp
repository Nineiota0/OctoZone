#pragma once

#include "octozone/GoalPlanner.hpp"
#include "octozone/Grid.hpp"
#include "octozone/HidePlanner.hpp"
#include "octozone/OctopusMemory.hpp"
#include "octozone/Octopus.hpp"
#include "octozone/Path.hpp"
#include "octozone/Shark.hpp"

#include <optional>
#include <vector>

namespace octozone
{
    class OctopusBrain
    {
    public:
        void update(Octopus& octopus, const std::vector<Shark>& sharks, const Grid& grid);

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
            const std::vector<Shark>& sharks) const;

        int scoreMove(
            const Grid& grid,
            const Octopus& octopus,
            const std::vector<Shark>& sharks,
            Position move) const;

        bool executePlannedMove(
            const Grid& grid,
            Octopus& octopus,
            const std::vector<Shark>& actualSharks,
            const std::vector<Shark>& knownSharks) const;

        std::optional<Position> chooseSurvivalMove(
            const Grid& grid,
            const Octopus& octopus,
            const std::vector<Shark>& actualSharks,
            const std::vector<Shark>& knownSharks) const;

        bool isImmediatelySafeMove(
            const Grid& grid,
            const std::vector<Shark>& sharks,
            Position move) const;

        bool isLooping(const Octopus& octopus) const;

        GoalPlanner goalPlanner_;
        HidePlanner hidePlanner_;
        OctopusMemory memory_;
    };
}
