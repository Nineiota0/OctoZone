#pragma once

#include "octozone/Path.hpp"
#include "octozone/Position.hpp"
#include "octozone/SharkState.hpp"

namespace octozone
{

    class Shark
    {
    public:
        Shark(Position start, const Path& patrolRoute);

        Position getPosition() const;
        Position getDirection() const;

        SharkState getState() const;
        void setState(SharkState state);
        bool isChasing() const;

        void moveOneStep();
        void moveTo(Position position);

    private:
        Position position_;
        Path patrolRoute_;

        int patrolIndex_{0};
        int patrolDirection_{1};

        Position direction_{0, 0};

        SharkState state_{SharkState::Patrol};
    };

}