#pragma once

#include "octozone/DebugInfo.hpp"
#include "octozone/GameResult.hpp"
#include "octozone/Grid.hpp"
#include "octozone/Octopus.hpp"
#include "octozone/Shark.hpp"

#include <vector>

namespace octozone
{
    class IRenderer
    {
    public:
        virtual ~IRenderer() = default;

        virtual void clear() = 0;

        virtual void draw(
            const Grid& grid,
            const Octopus& octopus,
            const std::vector<Shark>& sharks,
            const DebugInfo& debugInfo) = 0;

        virtual void drawResult(GameResult result) = 0;
    };
}
