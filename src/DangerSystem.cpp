#include "octozone/DangerSystem.hpp"
#include "octozone/VisionSystem.hpp"

#include <algorithm>

namespace octozone
{
    namespace
    {
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

    Path DangerSystem::buildDangerPositions(
        const Grid& grid,
        const Octopus& octopus,
        const std::vector<Shark>& sharks)
    {
        Path danger;

        for (const Shark& shark : sharks)
        {
            Path currentDanger = buildDangerPositionsForShark(
                grid,
                shark.getPosition(),
                shark.getDirection(),
                shark.isChasing());

            for (Position position : currentDanger)
            {
                addUniquePosition(danger, position);
            }

            Shark::Projection projection = shark.projectAfterOctopusMove(
                grid,
                octopus.getPosition());

            Path projectedDanger = buildDangerPositionsForShark(
                grid,
                projection.position,
                projection.direction,
                projection.state == SharkState::Chase);

            for (Position position : projectedDanger)
            {
                addUniquePosition(danger, position);
            }
        }

        return danger;
    }

    Path DangerSystem::buildDangerPositionsForShark(
        const Grid& grid,
        const Position& sharkPosition,
        const Position& sharkDirection,
        bool includeCapturePositions)
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

    Path DangerSystem::buildEscapeRiskPositions(
        const Grid& grid,
        const Octopus& octopus,
        const std::vector<Shark>& sharks)
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

        for (const Shark& shark : sharks)
        {
            addSharkCollisionRisk(shark.getPosition());

            Shark::Projection projection = shark.projectAfterOctopusMove(
                grid,
                octopus.getPosition());

            addSharkCollisionRisk(projection.position);
        }

        return blockedPositions;
    }

    bool DangerSystem::isSafePosition(
        const Grid& grid,
        const std::vector<Shark>& sharks,
        const Position& position,
        const Path& danger)
    {
        if (grid.getTile(position) == Tile::Seaweed)
        {
            for (const Shark& shark : sharks)
            {
                if (position == shark.getPosition())
                {
                    return false;
                }
            }

            return true;
        }

        return !containsPosition(danger, position);
    }
}
