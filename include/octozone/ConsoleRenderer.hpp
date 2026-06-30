#pragma once

#include "octozone/Grid.hpp"

namespace octozone 
{

    class ConsoleRenderer 
    {
        public:
            void draw(const Grid& grid) const;
        
        private:
            char tileToChar(Tile tile) const;
    };

}