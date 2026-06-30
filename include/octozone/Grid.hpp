#pragma once

#include "octozone/Position.hpp"
#include "octozone/Tile.hpp"

#include <vector>

namespace octozone 
{
    class Grid 
    {
        public:
            Grid(int rows, int cols);

            int getRows() const;
            int getCols() const;

            bool isInBounds(Position position) const;
            Tile getTile(Position position) const;
            void setTile(Position position, Tile tile);

        private:
            int rows_;
            int cols_;
            std::vector<std::vector<Tile>> tiles_;
    };
}