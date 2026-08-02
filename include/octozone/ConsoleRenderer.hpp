#pragma once

#include "octozone/Grid.hpp"
#include "octozone/IRenderer.hpp"
#include "octozone/Octopus.hpp"
#include "octozone/Shark.hpp"

#include <vector>

namespace octozone
{

    class ConsoleRenderer : public IRenderer
    {
    public:
        void clear() override;

        void draw(
            const Grid& grid,
            const Octopus& octopus,
            const std::vector<Shark>& sharks,
            const DebugInfo& debugInfo) override;

        void drawResult(GameResult result) override;

    private:
        void printDebugInfo(const DebugInfo& debugInfo) const;
        const char* decisionToText(OctopusDecision decision) const;
        const char* sharkStateToText(SharkState state) const;
        void printTile(Tile tile) const;
    };

}
