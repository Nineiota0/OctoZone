#pragma once

#include "octozone/Grid.hpp"
#include "octozone/Octopus.hpp"
#include "octozone/Path.hpp"
#include "octozone/Position.hpp"
#include "octozone/Shark.hpp"

#include <vector>

namespace octozone
{
    class DangerSystem
    {
    public:
        static Path buildDangerPositions(
            const Grid& grid,
            const Octopus& octopus,
            const std::vector<Shark>& sharks);

        static Path buildDangerPositionsForShark(
            const Grid& grid,
            const Position& sharkPosition,
            const Position& sharkDirection,
            bool includeCapturePositions);

        static Path buildEscapeRiskPositions(
            const Grid& grid,
            const Octopus& octopus,
            const std::vector<Shark>& sharks);

        static bool isSafePosition(
            const Grid& grid,
            const std::vector<Shark>& sharks,
            const Position& position,
            const Path& danger);
    };
}
