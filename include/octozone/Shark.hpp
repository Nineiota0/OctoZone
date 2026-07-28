#pragma once

#include "octozone/Grid.hpp"
#include "octozone/Path.hpp"
#include "octozone/Position.hpp"
#include "octozone/SharkState.hpp"

#include <optional>

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
        void beginChase(Position octopusPosition);
        void beginSearch(Position lastKnownOctopusPosition, int turns);
        void rememberOctopusPosition(Position octopusPosition);
        std::optional<Position> getLastKnownOctopusPosition() const;
        int getSearchTurnsRemaining() const;
        void setSearchTurnsRemaining(int turns);
        void decrementSearchTurns();
        bool isChasing() const;
        bool isOnPatrolRoute() const;
        bool canDetect(const Grid& grid, Position octopusPosition, int range = 3) const;

        void moveOneStep(const Path& blockedPositions = {});
        void moveTo(Position position);
        void syncPatrolToPosition(Position position);
        Path findPathToNearestPatrolPoint(
            const Grid& grid,
            const Path& blockedPositions = {}) const;
        bool moveTowardPatrolRoute(
            const Grid& grid,
            const Path& blockedPositions = {});
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
        std::optional<Position> lastKnownOctopusPosition_;
        int searchTurnsRemaining_{0};
    };

}
