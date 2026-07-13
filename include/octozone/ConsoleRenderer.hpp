#pragma once

#include "octozone/Grid.hpp"
#include "octozone/Octopus.hpp"
#include "octozone/Shark.hpp"

namespace octozone
{

    class ConsoleRenderer
    {
    public:
        void draw(
            const Grid& grid,
            const Octopus& octopus,
            const Shark& shark) const;

    private:
        char tileToChar(Tile tile) const;
    };

}