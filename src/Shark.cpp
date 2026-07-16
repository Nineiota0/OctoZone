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
    }

    Shark::Shark(Position start, const Path& patrolRoute)
        : position_(start),
          patrolRoute_(patrolRoute)
    {
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
        bool octopusHidden)
    {
        if (isChasing() && octopusHidden)
        {
            setState(SharkState::Patrol);
        }

        if (isChasing())
        {
            Path chasePath = Pathfinder::findPath(
                grid,
                position_,
                octopusPosition);

            if (!chasePath.empty())
            {
                moveTo(chasePath.front());
            }

            return;
        }

        if (!moveTowardPatrolRoute(grid))
        {
            moveOneStep();
        }
    }

    void Shark::moveOneStep()
    {
        if (patrolRoute_.empty())
        {
            return;
        }

        if (patrolRoute_.size() == 1)
        {
            position_ = patrolRoute_.front();
            return;
        }

        Position oldPosition = position_;

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

        position_ = patrolRoute_[patrolIndex_];

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

    Path Shark::findPathToNearestPatrolPoint(const Grid& grid) const
    {
        Path bestPath;

        for (Position position : patrolRoute_)
        {
            Path path = Pathfinder::findPath(grid, position_, position);

            if (!path.empty() &&
                (bestPath.empty() || path.size() < bestPath.size()))
            {
                bestPath = path;
            }
        }

        return bestPath;
    }

    bool Shark::moveTowardPatrolRoute(const Grid& grid)
    {
        if (isOnPatrolRoute())
        {
            syncPatrolToPosition(position_);
            return false;
        }

        Path returnPath = findPathToNearestPatrolPoint(grid);

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
