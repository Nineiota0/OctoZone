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
            const std::vector<Shark>& sharks) override;

        void drawResult(bool playerWon, bool timedOut) override;

    private:
        void printDebugInfo(const Octopus& octopus) const;
        const char* decisionToText(OctopusDecision decision) const;
        void printTile(Tile tile) const;
    };

}
