#pragma once

#include "octozone/Grid.hpp"
#include "octozone/Octopus.hpp"
#include "octozone/Shark.hpp"

#include <vector>

namespace octozone
{

    class ConsoleRenderer
    {
    public:
        void draw(
            const Grid& grid,
            const Octopus& octopus,
            const std::vector<Shark>& sharks) const;

    private:
        char tileToChar(Tile tile) const;
        void printTile(Tile tile) const;
    };

}