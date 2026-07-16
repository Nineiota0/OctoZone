#include "octozone/Octopus.hpp"
#include "octozone/Pathfinder.hpp"
#include "octozone/VisionSystem.hpp"

#include <algorithm>
#include <cstdlib>
#include <limits>

namespace octozone
{
    namespace
    {
        int manhattanDistance(const Position& a, const Position& b)
        {
            return std::abs(a.row - b.row) + std::abs(a.col - b.col);
        }

        bool containsPosition(const Path& positions, const Position& position)
        {
            return std::find(
                positions.begin(),
                positions.end(),
                position) != positions.end();
        }

        void addUniquePosition(Path& positions, Position position)
        {
            if (!containsPosition(positions, position))
            {
                positions.push_back(position);
            }
        }
    }

    Octopus::Octopus(Position start, Position goal)
        : position_(start),
          goal_(goal)
    {
    }

    Position Octopus::getPosition() const
    {
        return position_;
    }

    Position Octopus::getGoal() const
    {
        return goal_;
    }

    bool Octopus::isHidden(const Grid& grid) const
    {
        return grid.getTile(position_) == Tile::Seaweed;
    }

    void Octopus::update(const Grid& grid, Shark& shark)
    {
        if (isHidden(grid))
        {
            shark.setState(SharkState::Patrol);
        }

        std::optional<Position> tacticalMove = chooseTacticalMove(grid, shark);

        if (tacticalMove.has_value())
        {
            Position nextPosition = tacticalMove.value();
            clearHideTarget();

            if (grid.getTile(nextPosition) == Tile::Seaweed)
            {
                setHideTarget(nextPosition);
                setDecision(OctopusDecision::Hide);
            }
            else
            {
                setDecision(OctopusDecision::MoveToGoal);
            }

            setPath(Path{nextPosition});
            moveOneStepWithMemory();
            return;
        }

        Path danger = buildDangerPositions(grid, shark);
        Path loopAwareDanger = addRecentLoopAvoidance(grid, danger);

        if (decision_ == OctopusDecision::Hide && hideTarget_.has_value())
        {
            Position hideTarget = hideTarget_.value();

            if (position_ == hideTarget)
            {
                clearHideTarget();
                setDecision(OctopusDecision::Wait);
            }
            else
            {
                Path pathToHideTarget = Pathfinder::findPath(
                    grid,
                    position_,
                    hideTarget,
                    isLooping() ? loopAwareDanger : danger);

                if (!pathToHideTarget.empty())
                {
                    setPath(pathToHideTarget);
                    moveOneStepWithMemory();
                    return;
                }
            }

            clearHideTarget();
            setDecision(OctopusDecision::Wait);
        }

        Path safePathToGoal = Pathfinder::findPath(
            grid,
            position_,
            goal_,
            loopAwareDanger);

        if (safePathToGoal.empty())
        {
            safePathToGoal = Pathfinder::findPath(
                grid,
                position_,
                goal_,
                danger);
        }

        if (!safePathToGoal.empty())
        {
            clearHideTarget();
            setDecision(OctopusDecision::MoveToGoal);
            setPath(safePathToGoal);
            moveOneStepWithMemory();
            return;
        }

        if (isHidden(grid))
        {
            setDecision(OctopusDecision::Wait);
            return;
        }

        Path safePathToSeaweed = findPathToBestSeaweed(
            grid,
            shark,
            loopAwareDanger);

        if (safePathToSeaweed.empty())
        {
            safePathToSeaweed = findPathToBestSeaweed(grid, shark, danger);
        }

        if (!safePathToSeaweed.empty())
        {
            Position hideTarget = safePathToSeaweed.back();

            setHideTarget(hideTarget);
            setDecision(OctopusDecision::Hide);
            setPath(safePathToSeaweed);
            moveOneStepWithMemory();
            return;
        }

        Path riskyPathToSeaweed = findRiskyPathToBestSeaweed(
            grid,
            shark,
            addRecentLoopAvoidance(grid, buildEscapeRiskPositions(grid, shark)));

        if (riskyPathToSeaweed.empty())
        {
            riskyPathToSeaweed = findRiskyPathToBestSeaweed(
                grid,
                shark,
                buildEscapeRiskPositions(grid, shark));
        }

        if (!riskyPathToSeaweed.empty())
        {
            Position hideTarget = riskyPathToSeaweed.back();

            setHideTarget(hideTarget);
            setDecision(OctopusDecision::Hide);
            setPath(riskyPathToSeaweed);
            moveOneStepWithMemory();
            return;
        }

        setDecision(OctopusDecision::Wait);
    }

    void Octopus::setPath(Path path)
    {
        path_ = path;
    }

    bool Octopus::hasPath() const
    {
        return !path_.empty();
    }

    void Octopus::moveOneStep()
    {
        if (path_.empty())
        {
            return;
        }

        position_ = path_.front();
        path_.erase(path_.begin());
    }

    OctopusDecision Octopus::getDecision() const
    {
        return decision_;
    }

    void Octopus::setDecision(OctopusDecision decision)
    {
        decision_ = decision;
    }

    std::optional<Position> Octopus::getHideTarget() const
    {
        return hideTarget_;
    }

    void Octopus::setHideTarget(Position position)
    {
        hideTarget_ = position;
    }

    void Octopus::clearHideTarget()
    {
        hideTarget_.reset();
    }

    Path Octopus::buildDangerPositions(const Grid& grid, const Shark& shark) const
    {
        Path danger = buildDangerPositionsForShark(
            grid,
            shark.getPosition(),
            shark.getDirection(),
            shark.isChasing());

        Shark::Projection projection = shark.projectAfterOctopusMove(
            grid,
            position_);

        Path projectedDanger = buildDangerPositionsForShark(
            grid,
            projection.position,
            projection.direction,
            projection.state == SharkState::Chase);

        for (Position position : projectedDanger)
        {
            addUniquePosition(danger, position);
        }

        return danger;
    }

    Path Octopus::buildDangerPositionsForShark(
        const Grid& grid,
        const Position& sharkPosition,
        const Position& sharkDirection,
        bool includeCapturePositions) const
    {
        Path danger = VisionSystem::getVisiblePositions(
            grid,
            sharkPosition,
            sharkDirection,
            3);

        addUniquePosition(danger, sharkPosition);

        if (includeCapturePositions)
        {
            Path capturePositions{
                {sharkPosition.row - 1, sharkPosition.col},
                {sharkPosition.row + 1, sharkPosition.col},
                {sharkPosition.row, sharkPosition.col - 1},
                {sharkPosition.row, sharkPosition.col + 1}
            };

            for (Position position : capturePositions)
            {
                if (grid.isInBounds(position) &&
                    grid.getTile(position) != Tile::Wall)
                {
                    addUniquePosition(danger, position);
                }
            }
        }

        return danger;
    }

    Path Octopus::buildEscapeRiskPositions(
        const Grid& grid,
        const Shark& shark) const
    {
        Path blockedPositions;

        auto addSharkCollisionRisk = [&grid, &blockedPositions](
            Position sharkPosition)
        {
            addUniquePosition(blockedPositions, sharkPosition);

            Path capturePositions{
                {sharkPosition.row - 1, sharkPosition.col},
                {sharkPosition.row + 1, sharkPosition.col},
                {sharkPosition.row, sharkPosition.col - 1},
                {sharkPosition.row, sharkPosition.col + 1}
            };

            for (Position position : capturePositions)
            {
                if (grid.isInBounds(position) &&
                    grid.getTile(position) != Tile::Wall)
                {
                    addUniquePosition(blockedPositions, position);
                }
            }
        };

        addSharkCollisionRisk(shark.getPosition());

        Shark::Projection projection = shark.projectAfterOctopusMove(
            grid,
            position_);

        addSharkCollisionRisk(projection.position);

        return blockedPositions;
    }

    Path Octopus::addRecentLoopAvoidance(
        const Grid& grid,
        const Path& blockedPositions) const
    {
        Path loopAwareBlockedPositions = blockedPositions;

        for (Position position : recentPositions_)
        {
            if (position == position_ ||
                position == goal_ ||
                grid.getTile(position) == Tile::Seaweed)
            {
                continue;
            }

            addUniquePosition(loopAwareBlockedPositions, position);
        }

        return loopAwareBlockedPositions;
    }

    Path Octopus::getMoveOptions(const Grid& grid) const
    {
        Path options{
            position_,
            {position_.row - 1, position_.col},
            {position_.row + 1, position_.col},
            {position_.row, position_.col - 1},
            {position_.row, position_.col + 1}
        };

        Path validOptions;

        for (Position position : options)
        {
            if (!grid.isInBounds(position) ||
                grid.getTile(position) == Tile::Wall)
            {
                continue;
            }

            validOptions.push_back(position);
        }

        return validOptions;
    }

    Path Octopus::findPathToBestSeaweed(
        const Grid& grid,
        const Shark& shark,
        const Path& danger) const
    {
        Path bestPath;

        for (int row = 0; row < grid.getRows(); ++row)
        {
            for (int col = 0; col < grid.getCols(); ++col)
            {
                Position position{row, col};

                if (grid.getTile(position) != Tile::Seaweed ||
                    !isSafePosition(grid, shark, position, danger))
                {
                    continue;
                }

                Path path = Pathfinder::findPath(
                    grid,
                    position_,
                    position,
                    danger);

                if (!path.empty() &&
                    (bestPath.empty() ||
                     path.size() < bestPath.size() ||
                     (path.size() == bestPath.size() &&
                      manhattanDistance(position, goal_) <
                          manhattanDistance(bestPath.back(), goal_))))
                {
                    bestPath = path;
                }
            }
        }

        return bestPath;
    }

    Path Octopus::findRiskyPathToBestSeaweed(
        const Grid& grid,
        const Shark&,
        const Path& blockedPositions) const
    {
        Path bestPath;

        for (int row = 0; row < grid.getRows(); ++row)
        {
            for (int col = 0; col < grid.getCols(); ++col)
            {
                Position position{row, col};

                if (grid.getTile(position) != Tile::Seaweed ||
                    containsPosition(blockedPositions, position))
                {
                    continue;
                }

                Path path = Pathfinder::findPath(
                    grid,
                    position_,
                    position,
                    blockedPositions);

                if (!path.empty() &&
                    (bestPath.empty() ||
                     path.size() < bestPath.size() ||
                     (path.size() == bestPath.size() &&
                      manhattanDistance(position, goal_) <
                          manhattanDistance(bestPath.back(), goal_))))
                {
                    bestPath = path;
                }
            }
        }

        return bestPath;
    }

    Path Octopus::findPathToNearestSeaweedFrom(
        const Grid& grid,
        Position start,
        const Path& blockedPositions) const
    {
        Path bestPath;

        for (int row = 0; row < grid.getRows(); ++row)
        {
            for (int col = 0; col < grid.getCols(); ++col)
            {
                Position position{row, col};

                if (grid.getTile(position) != Tile::Seaweed ||
                    containsPosition(blockedPositions, position))
                {
                    continue;
                }

                Path path = Pathfinder::findPath(
                    grid,
                    start,
                    position,
                    blockedPositions);

                if (!path.empty() &&
                    (bestPath.empty() ||
                     path.size() < bestPath.size() ||
                     (path.size() == bestPath.size() &&
                      manhattanDistance(position, goal_) <
                          manhattanDistance(bestPath.back(), goal_))))
                {
                    bestPath = path;
                }
            }
        }

        return bestPath;
    }

    std::optional<Position> Octopus::chooseTacticalMove(
        const Grid& grid,
        const Shark& shark) const
    {
        std::optional<Position> bestMove;
        int bestScore = std::numeric_limits<int>::min();

        for (Position move : getMoveOptions(grid))
        {
            int score = scoreMove(grid, shark, move);

            if (!bestMove.has_value() ||
                score > bestScore ||
                (score == bestScore &&
                 manhattanDistance(move, goal_) <
                     manhattanDistance(bestMove.value(), goal_)))
            {
                bestMove = move;
                bestScore = score;
            }
        }

        if (bestScore <= std::numeric_limits<int>::min() / 4)
        {
            return std::nullopt;
        }

        return bestMove;
    }

    int Octopus::scoreMove(
        const Grid& grid,
        const Shark& shark,
        Position move) const
    {
        Shark::Projection sharkProjection = shark.projectAfterOctopusMove(
            grid,
            move);

        if (move == shark.getPosition() ||
            move == sharkProjection.position)
        {
            return std::numeric_limits<int>::min() / 2;
        }

        bool visibleAfterSharkMoves = VisionSystem::canDetect(
            grid,
            sharkProjection.position,
            sharkProjection.direction,
            move,
            3);
        bool chasedAfterMove = sharkProjection.state == SharkState::Chase;
        bool pressuredAfterMove = visibleAfterSharkMoves || chasedAfterMove;

        int currentGoalDistance = manhattanDistance(position_, goal_);
        int nextGoalDistance = manhattanDistance(move, goal_);

        int score = 0;
        score += (currentGoalDistance - nextGoalDistance) * 140;
        score -= nextGoalDistance * 8;

        Path directGoalPath = Pathfinder::findPath(grid, move, goal_);

        if (!directGoalPath.empty())
        {
            score -= static_cast<int>(directGoalPath.size()) * 3;
        }

        if (move == goal_)
        {
            score += 10000;
        }

        if (move == position_)
        {
            score -= isHidden(grid) ? 220 : 160;
        }

        int recentVisits = static_cast<int>(std::count(
            recentPositions_.begin(),
            recentPositions_.end(),
            move));
        score -= recentVisits * 120;

        if (isLooping() && containsPosition(recentPositions_, move))
        {
            score -= 220;
        }

        if (grid.getTile(move) == Tile::Seaweed)
        {
            if (shark.isChasing() || pressuredAfterMove)
            {
                score += 520;
            }
            else
            {
                score += 30;
            }
        }

        if (isHidden(grid) &&
            !shark.isChasing() &&
            !pressuredAfterMove &&
            move != position_)
        {
            score += 180;
        }

        if (pressuredAfterMove)
        {
            Path escapeBlockedPositions = buildEscapeRiskPositions(grid, shark);
            addUniquePosition(escapeBlockedPositions, sharkProjection.position);

            Path escapePath = findPathToNearestSeaweedFrom(
                grid,
                move,
                addRecentLoopAvoidance(grid, escapeBlockedPositions));

            if (escapePath.empty())
            {
                escapePath = findPathToNearestSeaweedFrom(
                    grid,
                    move,
                    escapeBlockedPositions);
            }

            if (!escapePath.empty())
            {
                score += 220;
                score -= static_cast<int>(escapePath.size()) * 24;
            }
            else
            {
                score -= 260;
            }
        }
        else
        {
            score += 90;
        }

        if (!shark.isChasing() && !shark.isOnPatrolRoute())
        {
            score += 90;
        }

        return score;
    }

    bool Octopus::isSafePosition(
        const Grid& grid,
        const Shark& shark,
        const Position& position,
        const Path& danger) const
    {
        if (grid.getTile(position) == Tile::Seaweed)
        {
            return position != shark.getPosition();
        }

        return !containsPosition(danger, position);
    }

    bool Octopus::isLooping() const
    {
        return std::count(
            recentPositions_.begin(),
            recentPositions_.end(),
            position_) >= 2;
    }

    void Octopus::moveOneStepWithMemory()
    {
        recentPositions_.push_back(position_);

        if (recentPositions_.size() > 12)
        {
            recentPositions_.erase(recentPositions_.begin());
        }

        moveOneStep();
    }
}
