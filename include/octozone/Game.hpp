#pragma once

#include "octozone/ConsoleRenderer.hpp"
#include "octozone/Grid.hpp"
#include "octozone/Octopus.hpp"
#include "octozone/Shark.hpp"

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
        Shark shark_;

        void initializeMap();
        void update();
        void render();
    };

}