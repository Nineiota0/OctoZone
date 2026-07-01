#pragma once

#include "octozone/ConsoleRenderer.hpp"
#include "octozone/Grid.hpp"

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
        
            void initializeMap();
    };

}