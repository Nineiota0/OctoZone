#include "octozone/Shark.hpp"
#include "octozone/Pathfinder.hpp"
#include "octozone/VisionSystem.hpp"

#include <algorithm>

namespace octozone
{
    namespace
    {
        int nextPatrolIndex(int currentIndex, int direction, int routeSize)
        {
            if (routeSize <= 1)
            {
                return currentIndex;
            }

            int nextIndex = currentIndex + direction;

            if (nextIndex >= routeSize)
            {
                return routeSize - 2;
            }

            if (nextIndex < 0)
            {
                return 1;
            }

            return nextIndex;
        }

        bool containsPosition(const Path& positions, const Position& position)
        {
            return std::find(
                positions.begin(),
                positions.end(),
                position
            ) != positions.end();
        }
    }

    Shark::Shark(Position start, const Path& patrolRoute)
        : position_(start),
          patrolRoute_(patrolRoute)
    {
        if (patrolRoute_.size() > 1)
        {
            direction_ = {
                patrolRoute_[1].row - patrolRoute_[0].row,
                patrolRoute_[1].col - patrolRoute_[0].col
            };
        }
    }

    Position Shark::getPosition() const
    {
        return position_;
    }

    Position Shark::getDirection() const
    {
        return direction_;
    }

    const Path& Shark::getPatrolRoute() const
    {
        return patrolRoute_;
    }

    Position Shark::getNextPatrolPosition() const
    {
        if (patrolRoute_.empty())
        {
            return position_;
        }

        return patrolRoute_[nextPatrolIndex(
            patrolIndex_,
            patrolDirection_,
            static_cast<int>(patrolRoute_.size()))];
    }

    Position Shark::getNextPatrolDirection() const
    {
        Position nextPosition = getNextPatrolPosition();

        return {
            nextPosition.row - position_.row,
            nextPosition.col - position_.col
        };
    }

    SharkState Shark::getState() const
    {
        return state_;
    }

    void Shark::setState(SharkState state)
    {
        state_ = state;

        if (state_ == SharkState::Patrol)
        {
            lastKnownOctopusPosition_.reset();
        }
    }

    void Shark::beginChase(Position octopusPosition)
    {
        state_ = SharkState::Chase;
        lastKnownOctopusPosition_ = octopusPosition;
    }

    bool Shark::isChasing() const
    {
        return state_ == SharkState::Chase;
    }

    bool Shark::isOnPatrolRoute() const
    {
        return std::find(
            patrolRoute_.begin(),
            patrolRoute_.end(),
            position_) != patrolRoute_.end();
    }

    bool Shark::canDetect(
        const Grid& grid,
        Position octopusPosition,
        int range) const
    {
        return VisionSystem::canDetect(
            grid,
            position_,
            direction_,
            octopusPosition,
            range);
    }

    void Shark::update(
        const Grid& grid,
        Position octopusPosition,
        bool octopusHidden,
        const Path& occupiedPositions)
    {
        bool canSeeOctopus =
            !octopusHidden &&
            canDetect(grid, octopusPosition);

        if (canSeeOctopus)
        {
            beginChase(octopusPosition);
        }

        if (isChasing() && octopusHidden)
        {
            setState(SharkState::Patrol);
        }

        if (isChasing())
        {
            lastKnownOctopusPosition_ = octopusPosition;

            Path chasePath = Pathfinder::findPath(
                grid,
                position_,
                octopusPosition,
                occupiedPositions);

            if (!chasePath.empty())
            {
                moveTo(chasePath.front());
                return;
            }

            return;
        }

        lastKnownOctopusPosition_.reset();

        if (!moveTowardPatrolRoute(grid, occupiedPositions))
        {
            moveOneStep(occupiedPositions);
        }
    }

    void Shark::moveOneStep(const Path& blockedPositions)
    {
        if (patrolRoute_.empty())
        {
            return;
        }

        if (patrolRoute_.size() == 1)
        {
            if (!containsPosition(blockedPositions, patrolRoute_.front()))
            {
                position_ = patrolRoute_.front();
            }
            return;
        }

        Position oldPosition = position_;
        int oldPatrolIndex = patrolIndex_;
        int oldPatrolDirection = patrolDirection_;

        patrolIndex_ += patrolDirection_;

        if (patrolIndex_ >= static_cast<int>(patrolRoute_.size()))
        {
            patrolDirection_ = -1;
            patrolIndex_ = static_cast<int>(patrolRoute_.size()) - 2;
        }
        else if (patrolIndex_ < 0)
        {
            patrolDirection_ = 1;
            patrolIndex_ = 1;
        }

        Position nextPosition = patrolRoute_[patrolIndex_];

        if (containsPosition(blockedPositions, nextPosition))
        {
            patrolIndex_ = oldPatrolIndex;
            patrolDirection_ = oldPatrolDirection;
            return;
        }

        position_ = nextPosition;

        direction_ = {
            position_.row - oldPosition.row,
            position_.col - oldPosition.col
        };
    }

    void Shark::moveTo(Position position)
    {
        direction_ = {
            position.row - position_.row,
            position.col - position_.col
        };

        position_ = position;
    }

    void Shark::syncPatrolToPosition(Position position)
    {
        auto patrolPoint = std::find(
            patrolRoute_.begin(),
            patrolRoute_.end(),
            position);

        if (patrolPoint == patrolRoute_.end())
        {
            return;
        }

        patrolIndex_ = static_cast<int>(patrolPoint - patrolRoute_.begin());
        position_ = position;
    }

    Path Shark::findPathToNearestPatrolPoint(
        const Grid& grid,
        const Path& blockedPositions) const
    {
        Path bestPath;

        for (Position position : patrolRoute_)
        {
            Path path = Pathfinder::findPath(
                grid,
                position_,
                position,
                blockedPositions);

            if (!path.empty() &&
                (bestPath.empty() || path.size() < bestPath.size()))
            {
                bestPath = path;
            }
        }

        return bestPath;
    }

    bool Shark::moveTowardPatrolRoute(
        const Grid& grid,
        const Path& blockedPositions)
    {
        if (isOnPatrolRoute())
        {
            syncPatrolToPosition(position_);
            return false;
        }

        Path returnPath = findPathToNearestPatrolPoint(
            grid,
            blockedPositions);

        if (returnPath.empty())
        {
            return false;
        }

        moveTo(returnPath.front());

        if (isOnPatrolRoute())
        {
            syncPatrolToPosition(position_);
        }

        return true;
    }

    Shark::Projection Shark::projectAfterOctopusMove(
        const Grid& grid,
        Position octopusPosition) const
    {
        Projection projection{
            position_,
            direction_,
            state_
        };

        if (projection.state == SharkState::Chase &&
            grid.getTile(octopusPosition) == Tile::Seaweed)
        {
            projection.state = SharkState::Patrol;
        }

        if (projection.state == SharkState::Chase)
        {
            Path chasePath = Pathfinder::findPath(
                grid,
                position_,
                octopusPosition);

            if (!chasePath.empty())
            {
                projection.position = chasePath.front();
                projection.direction = {
                    projection.position.row - position_.row,
                    projection.position.col - position_.col
                };
            }
        }
        else if (!isOnPatrolRoute())
        {
            Path returnPath = findPathToNearestPatrolPoint(grid);

            if (!returnPath.empty())
            {
                projection.position = returnPath.front();
                projection.direction = {
                    projection.position.row - position_.row,
                    projection.position.col - position_.col
                };
            }
        }
        else
        {
            projection.position = getNextPatrolPosition();
            projection.direction = getNextPatrolDirection();
        }

        bool canDetectOctopus = VisionSystem::canDetect(
                grid,
                projection.position,
                projection.direction,
                octopusPosition,
                3);

        if (projection.state == SharkState::Chase)
        {
            return projection;
        }

        if (canDetectOctopus)
        {
            projection.state = SharkState::Chase;
        }

        return projection;
    }

}
