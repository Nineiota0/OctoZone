#pragma once

#include "octozone/Grid.hpp"
#include "octozone/IRenderer.hpp"
#include "octozone/Octopus.hpp"
#include "octozone/OctopusBrain.hpp"
#include "octozone/Shark.hpp"
#include "octozone/SharkBrain.hpp"

#include <memory>
#include <vector>

namespace octozone
{
    class Game
    {
    public:
        Game();
        explicit Game(std::unique_ptr<IRenderer> renderer);

        void run();

    private:
        Grid grid_;
        std::unique_ptr<IRenderer> renderer_;
        Octopus octopus_;
        std::vector<Shark> sharks_;
        OctopusBrain octopusBrain_;
        SharkBrain sharkBrain_;

        bool gameOver_{false};
        bool playerWon_{false};
        bool timedOut_{false};
        int turnCount_{0};
        static constexpr int maxTurns_{350};

        void update();
        void resolveTurn();
        void render();

        void updateOctopus();
        void updateSharks();

        void refreshSharkChases();
        bool resolveCapture(
            Position previousOctopusPosition,
            const std::vector<Position>& previousSharkPositions,
            bool includeSwaps);
        void resolveGoal();
    };
}
