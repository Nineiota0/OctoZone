#include "octozone/SharkBrain.hpp"
#include "octozone/Pathfinder.hpp"

namespace octozone
{
    void SharkBrain::update(
        Shark& shark,
        const Grid& grid,
        Position octopusPosition,
        bool octopusHidden,
        const Path& occupiedPositions) const
    {
        bool canSeeOctopus =
            !octopusHidden &&
            shark.canDetect(grid, octopusPosition);

        if (canSeeOctopus)
        {
            shark.beginChase(octopusPosition);
        }

        switch (shark.getState())
        {
            case SharkState::Patrol:
                updatePatrol(shark, grid, occupiedPositions);
                break;

            case SharkState::Chase:
                updateChase(
                    shark,
                    grid,
                    octopusPosition,
                    octopusHidden,
                    occupiedPositions);
                break;

            case SharkState::Search:
                updateSearch(
                    shark,
                    grid,
                    octopusPosition,
                    octopusHidden,
                    occupiedPositions);
                break;

            case SharkState::ReturnToPatrol:
                updateReturnToPatrol(shark, grid, occupiedPositions);
                break;
        }
    }

    void SharkBrain::updatePatrol(
        Shark& shark,
        const Grid& grid,
        const Path& occupiedPositions) const
    {
        if (!shark.moveTowardPatrolRoute(grid, occupiedPositions))
        {
            shark.moveOneStep(occupiedPositions);
        }
    }

    void SharkBrain::updateChase(
        Shark& shark,
        const Grid& grid,
        Position octopusPosition,
        bool octopusHidden,
        const Path& occupiedPositions) const
    {
        if (octopusHidden)
        {
            shark.beginSearch(octopusPosition, searchTurns_);
            updateSearch(
                shark,
                grid,
                octopusPosition,
                octopusHidden,
                occupiedPositions);
            return;
        }

        shark.rememberOctopusPosition(octopusPosition);
        moveToward(shark, grid, octopusPosition, occupiedPositions);
    }

    void SharkBrain::updateSearch(
        Shark& shark,
        const Grid& grid,
        Position octopusPosition,
        bool octopusHidden,
        const Path& occupiedPositions) const
    {
        if (!octopusHidden &&
            shark.canDetect(grid, octopusPosition))
        {
            shark.beginChase(octopusPosition);
            updateChase(
                shark,
                grid,
                octopusPosition,
                octopusHidden,
                occupiedPositions);
            return;
        }

        if (shark.getLastKnownOctopusPosition().has_value() &&
            shark.getPosition() != shark.getLastKnownOctopusPosition().value())
        {
            moveToward(
                shark,
                grid,
                shark.getLastKnownOctopusPosition().value(),
                occupiedPositions);
            return;
        }

        shark.decrementSearchTurns();

        if (shark.getSearchTurnsRemaining() <= 0)
        {
            shark.setState(SharkState::ReturnToPatrol);
        }
    }

    void SharkBrain::updateReturnToPatrol(
        Shark& shark,
        const Grid& grid,
        const Path& occupiedPositions) const
    {
        if (shark.isOnPatrolRoute())
        {
            shark.setState(SharkState::Patrol);
            shark.syncPatrolToPosition(shark.getPosition());
            return;
        }

        if (!shark.moveTowardPatrolRoute(grid, occupiedPositions))
        {
            shark.moveOneStep(occupiedPositions);
        }

        if (shark.isOnPatrolRoute())
        {
            shark.setState(SharkState::Patrol);
            shark.syncPatrolToPosition(shark.getPosition());
        }
    }

    bool SharkBrain::moveToward(
        Shark& shark,
        const Grid& grid,
        Position target,
        const Path& occupiedPositions) const
    {
        Path path = Pathfinder::findPath(
            grid,
            shark.getPosition(),
            target,
            occupiedPositions);

        if (path.empty())
        {
            return false;
        }

        shark.moveTo(path.front());
        return true;
    }
}
