#pragma once

#include "octozone/Path.hpp"
#include "octozone/Position.hpp"

namespace octozone 
{

    class Shark 
    {
        public:
           Shark(Position start, const Path& patrolRoute);
        
            Position getPosition() const;
            void moveOneStep();

            Position getDirection() const;
        
        private:
            Position position_;
            Path patrolRoute_;

            int patrolIndex_{0};
            int patrolDirection_{1};

            Position direction_{0, 0};
    };

}