#pragma once

#include "octozone/Grid.hpp"
#include "octozone/OctopusDecision.hpp"
#include "octozone/Path.hpp"
#include "octozone/Position.hpp"
#include "octozone/Shark.hpp"

#include <optional>
#include <vector>

namespace octozone
{

    class Octopus
    {
    public:
        Octopus(Position start, Position goal);

        Position getPosition() const;
        Position getGoal() const;
        bool isHidden(const Grid& grid) const;

        void update(const Grid& grid, Shark& shark);
        void setPath(Path path);
        bool hasPath() const;
        void moveOneStep();

        OctopusDecision getDecision() const;
        void setDecision(OctopusDecision decision);

        std::optional<Position> getHideTarget() const;
        void setHideTarget(Position position);
        void clearHideTarget();

    private:
        Position position_;
        Position goal_;
        Path path_;
        std::vector<Position> recentPositions_;

        OctopusDecision decision_{OctopusDecision::MoveToGoal};
        std::optional<Position> hideTarget_;

        Path buildDangerPositions(const Grid& grid, const Shark& shark) const;
        Path buildDangerPositionsForShark(
            const Grid& grid,
            const Position& sharkPosition,
            const Position& sharkDirection,
            bool includeCapturePositions) const;
        Path buildEscapeRiskPositions(const Grid& grid, const Shark& shark) const;
        Path addRecentLoopAvoidance(
            const Grid& grid,
            const Path& blockedPositions) const;
        Path getMoveOptions(const Grid& grid) const;
        Path findPathToBestSeaweed(
            const Grid& grid,
            const Shark& shark,
            const Path& danger) const;
        Path findRiskyPathToBestSeaweed(
            const Grid& grid,
            const Shark& shark,
            const Path& blockedPositions) const;
        Path findPathToNearestSeaweedFrom(
            const Grid& grid,
            Position start,
            const Path& blockedPositions) const;
        std::optional<Position> chooseTacticalMove(
            const Grid& grid,
            const Shark& shark) const;
        int scoreMove(
            const Grid& grid,
            const Shark& shark,
            Position move) const;
        bool isSafePosition(
            const Grid& grid,
            const Shark& shark,
            const Position& position,
            const Path& danger) const;
        bool isLooping() const;
        void moveOneStepWithMemory();
    };

}
