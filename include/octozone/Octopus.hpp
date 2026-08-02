#pragma once

#include "octozone/Grid.hpp"
#include "octozone/OctopusDecision.hpp"
#include "octozone/Path.hpp"
#include "octozone/Position.hpp"

#include <optional>

namespace octozone
{
    class Octopus
    {
    public:
        Octopus(Position start, Position goal);

        Position getPosition() const;
        Position getGoal() const;
        bool isHidden(const Grid& grid) const;

        void setPath(Path path);
        bool hasPath() const;
        int getPathLength() const;
        std::optional<Position> getNextPathPosition() const;
        void moveOneStep();
        const Path& getRecentPositions() const;
        bool isOscillating() const;

        OctopusDecision getDecision() const;
        void setDecision(OctopusDecision decision);

        std::optional<Position> getHideTarget() const;
        void setHideTarget(Position position);
        void clearHideTarget();

    private:
        Position position_;
        Position goal_;
        Path path_;
        Path recentPositions_;

        OctopusDecision decision_{OctopusDecision::MoveToGoal};
        std::optional<Position> hideTarget_;
    };
}
