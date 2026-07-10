#pragma once

#include "octozone/Path.hpp"
#include "octozone/Position.hpp"

namespace octozone 
{

    class Octopus 
    {
        public:
            Octopus(Position start, Position goal);
        
            Position getPosition() const;
            Position getGoal() const;
        
            void setPath(Path path);
            bool hasPath() const;
            void moveOneStep();
        
        private:
            Position position_;
            Position goal_;
            Path path_;
    };

}