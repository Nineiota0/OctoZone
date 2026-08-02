#pragma once

#include "octozone/OctopusDecision.hpp"
#include "octozone/Position.hpp"
#include "octozone/SharkState.hpp"

#include <optional>
#include <vector>

namespace octozone
{
    struct SharkDebugInfo
    {
        SharkState state{SharkState::Patrol};
        std::optional<Position> lastKnownTarget;
        int searchTurnsRemaining{};
    };

    struct DebugInfo
    {
        OctopusDecision octopusDecision{OctopusDecision::MoveToGoal};
        std::optional<Position> octopusObjective;
        int octopusPathLength{};
        std::vector<SharkDebugInfo> sharks;
        unsigned int mapSeed{};
        int turnCount{};
        int maxTurns{};
    };
}
