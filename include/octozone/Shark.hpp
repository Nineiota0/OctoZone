#pragma once

#include "octozone/Grid.hpp"
#include "octozone/Path.hpp"
#include "octozone/Position.hpp"
#include "octozone/SharkState.hpp"

namespace octozone
{

    class Shark
    {
    public:
        struct Projection
        {
            Position position;
            Position direction;
            SharkState state;
        };

        Shark(Position start, const Path& patrolRoute);

        Position getPosition() const;
        Position getDirection() const;
        const Path& getPatrolRoute() const;
        Position getNextPatrolPosition() const;
        Position getNextPatrolDirection() const;

        SharkState getState() const;
        void setState(SharkState state);
        bool isChasing() const;
        bool isOnPatrolRoute() const;
        bool canDetect(const Grid& grid, Position octopusPosition, int range = 3) const;

        void update(const Grid& grid, Position octopusPosition, bool octopusHidden);
        void moveOneStep();
        void moveTo(Position position);
        void syncPatrolToPosition(Position position);
        Path findPathToNearestPatrolPoint(const Grid& grid) const;
        bool moveTowardPatrolRoute(const Grid& grid);
        Projection projectAfterOctopusMove(
            const Grid& grid,
            Position octopusPosition) const;

    private:
        Position position_;
        Path patrolRoute_;

        int patrolIndex_{0};
        int patrolDirection_{1};

        Position direction_{0, 0};

        SharkState state_{SharkState::Patrol};
    };

}
