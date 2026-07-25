#include "octozone/OctopusBrain.hpp"
#include "octozone/DangerSystem.hpp"
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
                position
            ) != positions.end();
        }

        void addUniquePosition(Path& positions, Position position)
        {
            if (!containsPosition(positions, position))
            {
                positions.push_back(position);
            }
        }
    }

    void OctopusBrain::update(
        Octopus& octopus,
        const std::vector<Shark>& sharks,
        const Grid& grid)
    {
        Path danger = DangerSystem::buildDangerPositions(grid, octopus, sharks);
        Path loopAwareDanger = addRecentLoopAvoidance(grid, octopus, danger);

        bool anySharkChasing = std::any_of(
            sharks.begin(),
            sharks.end(),
            [](const Shark& shark)
            {
                return shark.isChasing();
            });

        bool octopusThreatened =
            anySharkChasing ||
            containsPosition(danger, octopus.getPosition());

        if (octopus.getDecision() == OctopusDecision::Hide &&
            octopus.getHideTarget().has_value())
        {
            Position hideTarget = octopus.getHideTarget().value();

            if (octopus.getPosition() == hideTarget)
            {
                octopus.clearHideTarget();
                octopus.setDecision(OctopusDecision::MoveToGoal);
            }
            else
            {
                Path pathToHideTarget = Pathfinder::findPath(
                    grid,
                    octopus.getPosition(),
                    hideTarget,
                    isLooping(octopus) ? loopAwareDanger : danger);

                if (!pathToHideTarget.empty())
                {
                    octopus.setPath(pathToHideTarget);
                    moveOneStepWithMemory(octopus);
                    return;
                }

                octopus.clearHideTarget();
                octopus.setDecision(OctopusDecision::Wait);
            }
        }

        Path safePathToGoal = goalPlanner_.findPathToGoal(
            grid,
            octopus,
            loopAwareDanger);

        if (safePathToGoal.empty())
        {
            safePathToGoal = goalPlanner_.findPathToGoal(
                grid,
                octopus,
                danger);
        }

        if (!safePathToGoal.empty())
        {
            octopus.clearHideTarget();
            octopus.setDecision(OctopusDecision::MoveToGoal);
            octopus.setPath(safePathToGoal);
            moveOneStepWithMemory(octopus);
            return;
        }

        Path escapeRisk = DangerSystem::buildEscapeRiskPositions(
            grid,
            octopus,
            sharks);

        Path riskyPathToGoal = goalPlanner_.findPathToGoal(
            grid,
            octopus,
            isLooping(octopus)
                ? addRecentLoopAvoidance(grid, octopus, escapeRisk)
                : escapeRisk);

        if (riskyPathToGoal.empty() && isLooping(octopus))
        {
            riskyPathToGoal = goalPlanner_.findPathToGoal(
                grid,
                octopus,
                escapeRisk);
        }

        if (!riskyPathToGoal.empty() &&
            (isLooping(octopus) || !octopusThreatened) &&
            scoreMove(grid, octopus, sharks, riskyPathToGoal.front()) >
                std::numeric_limits<int>::min() / 4)
        {
            octopus.clearHideTarget();
            octopus.setDecision(OctopusDecision::MoveToGoal);
            octopus.setPath(riskyPathToGoal);
            moveOneStepWithMemory(octopus);
            return;
        }

        if (octopusThreatened)
        {
            Path safePathToSeaweed = hidePlanner_.findPathToBestSeaweed(
                grid,
                octopus,
                sharks,
                loopAwareDanger);

            if (safePathToSeaweed.empty())
            {
                safePathToSeaweed = hidePlanner_.findPathToBestSeaweed(
                    grid,
                    octopus,
                    sharks,
                    danger);
            }

            if (!safePathToSeaweed.empty())
            {
                Position hideTarget = safePathToSeaweed.back();

                octopus.setHideTarget(hideTarget);
                octopus.setDecision(OctopusDecision::Hide);
                octopus.setPath(safePathToSeaweed);
                moveOneStepWithMemory(octopus);
                return;
            }

            Path riskyPathToSeaweed = findRiskyPathToBestSeaweed(
                grid,
                octopus,
                addRecentLoopAvoidance(grid, octopus, escapeRisk));

            if (riskyPathToSeaweed.empty())
            {
                riskyPathToSeaweed = findRiskyPathToBestSeaweed(
                    grid,
                    octopus,
                    escapeRisk);
            }

            if (!riskyPathToSeaweed.empty())
            {
                Position hideTarget = riskyPathToSeaweed.back();

                octopus.setHideTarget(hideTarget);
                octopus.setDecision(OctopusDecision::Hide);
                octopus.setPath(riskyPathToSeaweed);
                moveOneStepWithMemory(octopus);
                return;
            }
        }

        std::optional<Position> tacticalMove =
            chooseTacticalMove(grid, octopus, sharks);

        if (tacticalMove.has_value())
        {
            Position nextPosition = tacticalMove.value();

            if (nextPosition == octopus.getPosition())
            {
                octopus.setDecision(OctopusDecision::Wait);
                return;
            }

            octopus.clearHideTarget();

            if (octopusThreatened &&
                grid.getTile(nextPosition) == Tile::Seaweed)
            {
                octopus.setHideTarget(nextPosition);
                octopus.setDecision(OctopusDecision::Hide);
            }
            else
            {
                octopus.setDecision(OctopusDecision::MoveToGoal);
            }

            octopus.setPath(Path{nextPosition});
            moveOneStepWithMemory(octopus);
            return;
        }

        octopus.setDecision(OctopusDecision::Wait);
    }

    Path OctopusBrain::addRecentLoopAvoidance(
        const Grid& grid,
        const Octopus& octopus,
        const Path& blockedPositions) const
    {
        Path loopAwareBlockedPositions = blockedPositions;

        for (Position position : recentPositions_)
        {
            if (position == octopus.getPosition() ||
                position == octopus.getGoal() ||
                grid.getTile(position) == Tile::Seaweed)
            {
                continue;
            }

            addUniquePosition(loopAwareBlockedPositions, position);
        }

        return loopAwareBlockedPositions;
    }

    Path OctopusBrain::getMoveOptions(
        const Grid& grid,
        const Octopus& octopus) const
    {
        Position position = octopus.getPosition();

        Path options{
            position,
            {position.row - 1, position.col},
            {position.row + 1, position.col},
            {position.row, position.col - 1},
            {position.row, position.col + 1}
        };

        Path validOptions;

        for (Position option : options)
        {
            if (!grid.isInBounds(option) ||
                grid.getTile(option) == Tile::Wall)
            {
                continue;
            }

            validOptions.push_back(option);
        }

        return validOptions;
    }

    Path OctopusBrain::findRiskyPathToBestSeaweed(
        const Grid& grid,
        const Octopus& octopus,
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
                    octopus.getPosition(),
                    position,
                    blockedPositions);

                if (!path.empty() &&
                    (bestPath.empty() ||
                     path.size() < bestPath.size() ||
                     (path.size() == bestPath.size() &&
                      manhattanDistance(position, octopus.getGoal()) <
                          manhattanDistance(bestPath.back(), octopus.getGoal()))))
                {
                    bestPath = path;
                }
            }
        }

        return bestPath;
    }

    Path OctopusBrain::findPathToNearestSeaweedFrom(
        const Grid& grid,
        const Octopus& octopus,
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
                      manhattanDistance(position, octopus.getGoal()) <
                          manhattanDistance(bestPath.back(), octopus.getGoal()))))
                {
                    bestPath = path;
                }
            }
        }

        return bestPath;
    }

    std::optional<Position> OctopusBrain::chooseTacticalMove(
        const Grid& grid,
        const Octopus& octopus,
        const std::vector<Shark>& sharks) const
    {
        std::optional<Position> bestMove;
        int bestScore = std::numeric_limits<int>::min();

        for (Position move : getMoveOptions(grid, octopus))
        {
            int score = scoreMove(grid, octopus, sharks, move);

            if (!bestMove.has_value() ||
                score > bestScore ||
                (score == bestScore &&
                 manhattanDistance(move, octopus.getGoal()) <
                     manhattanDistance(bestMove.value(), octopus.getGoal())))
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

    int OctopusBrain::scoreMove(
        const Grid& grid,
        const Octopus& octopus,
        const std::vector<Shark>& sharks,
        Position move) const
    {
        bool anySharkChasing = false;
        bool anySharkOffPatrol = false;
        bool pressuredAfterMove = false;
        Path escapeBlockedPositions =
            DangerSystem::buildEscapeRiskPositions(grid, octopus, sharks);

        for (const Shark& shark : sharks)
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
            pressuredAfterMove = pressuredAfterMove ||
                visibleAfterSharkMoves ||
                chasedAfterMove;
            anySharkChasing = anySharkChasing || shark.isChasing();
            anySharkOffPatrol = anySharkOffPatrol || !shark.isOnPatrolRoute();

            addUniquePosition(
                escapeBlockedPositions,
                sharkProjection.position);
        }

        int currentGoalDistance = manhattanDistance(
            octopus.getPosition(),
            octopus.getGoal());

        int nextGoalDistance = manhattanDistance(
            move,
            octopus.getGoal());

        int score = 0;
        score += (currentGoalDistance - nextGoalDistance) * 140;
        score -= nextGoalDistance * 8;

        Path directGoalPath = Pathfinder::findPath(
            grid,
            move,
            octopus.getGoal());

        if (!directGoalPath.empty())
        {
            score -= static_cast<int>(directGoalPath.size()) * 3;
        }

        if (move == octopus.getGoal())
        {
            score += 10000;
        }

        if (move == octopus.getPosition())
        {
            score -= octopus.isHidden(grid) ? 220 : 160;
        }

        int recentVisits = static_cast<int>(std::count(
            recentPositions_.begin(),
            recentPositions_.end(),
            move));

        score -= recentVisits * 120;

        if (isLooping(octopus) && containsPosition(recentPositions_, move))
        {
            score -= 220;
        }

        if (grid.getTile(move) == Tile::Seaweed)
        {
            if (anySharkChasing || pressuredAfterMove)
            {
                score += 520;
            }
            else
            {
                score += 30;
            }
        }

        if (octopus.isHidden(grid) &&
            !anySharkChasing &&
            !pressuredAfterMove &&
            move != octopus.getPosition())
        {
            score += 180;
        }

        if (pressuredAfterMove)
        {
            Path escapePath = findPathToNearestSeaweedFrom(
                grid,
                octopus,
                move,
                addRecentLoopAvoidance(grid, octopus, escapeBlockedPositions));

            if (escapePath.empty())
            {
                escapePath = findPathToNearestSeaweedFrom(
                    grid,
                    octopus,
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

        if (!anySharkChasing && anySharkOffPatrol)
        {
            score += 90;
        }

        return score;
    }

    bool OctopusBrain::isLooping(const Octopus& octopus) const
    {
        return std::count(
            recentPositions_.begin(),
            recentPositions_.end(),
            octopus.getPosition()) >= 2;
    }

    void OctopusBrain::moveOneStepWithMemory(Octopus& octopus)
    {
        recentPositions_.push_back(octopus.getPosition());

        if (recentPositions_.size() > 12)
        {
            recentPositions_.erase(recentPositions_.begin());
        }

        octopus.moveOneStep();
    }
}
