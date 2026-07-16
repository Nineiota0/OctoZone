#pragma once

#include "octozone/Grid.hpp"
#include "octozone/Octopus.hpp"
#include "octozone/Path.hpp"
#include "octozone/Position.hpp"
#include "octozone/Shark.hpp"

namespace octozone
{
    class DangerSystem
    {
    public:
        static Path buildDangerPositions(
            const Grid& grid,
            const Octopus& octopus,
            const Shark& shark);

        static Path buildDangerPositionsForShark(
            const Grid& grid,
            const Position& sharkPosition,
            const Position& sharkDirection,
            bool includeCapturePositions);

        static Path buildEscapeRiskPositions(
            const Grid& grid,
            const Octopus& octopus,
            const Shark& shark);

        static bool isSafePosition(
            const Grid& grid,
            const Shark& shark,
            const Position& position,
            const Path& danger);
    };
}