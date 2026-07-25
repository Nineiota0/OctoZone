#pragma once

#include "octozone/ConsoleRenderer.hpp"
#include "octozone/Grid.hpp"
#include "octozone/Octopus.hpp"
#include "octozone/OctopusBrain.hpp"
#include "octozone/Shark.hpp"

#include <vector>

namespace octozone
{
    class Game
    {
    public:
        Game();

        void run();

    private:
        Grid grid_;
        ConsoleRenderer renderer_;
        Octopus octopus_;
        std::vector<Shark> sharks_;
        OctopusBrain octopusBrain_;

        bool gameOver_{false};
        bool playerWon_{false};

        void initializeMap();
        void update();
        void render();

        void updateOctopus();
        void updateSharks();

        void refreshSharkChases();
        void checkWinCondition();
        void checkLoseCondition();
    };
}
