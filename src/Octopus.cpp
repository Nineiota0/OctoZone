#include "octozone/Octopus.hpp"

namespace octozone
{
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

    void Octopus::setPath(Path path)
    {
        path_ = path;
    }

    bool Octopus::hasPath() const
    {
        return !path_.empty();
    }

    std::optional<Position> Octopus::getNextPathPosition() const
    {
        if (path_.empty())
        {
            return std::nullopt;
        }

        return path_.front();
    }

    void Octopus::moveOneStep()
    {
        if (path_.empty())
        {
            return;
        }

        recentPositions_.push_back(position_);

        if (recentPositions_.size() > 12)
        {
            recentPositions_.erase(recentPositions_.begin());
        }

        position_ = path_.front();
        path_.erase(path_.begin());
    }

    const Path& Octopus::getRecentPositions() const
    {
        return recentPositions_;
    }

    bool Octopus::isOscillating() const
    {
        if (recentPositions_.size() < 4)
        {
            return false;
        }

        std::size_t size = recentPositions_.size();

        return recentPositions_[size - 1] == recentPositions_[size - 3] &&
               recentPositions_[size - 2] == recentPositions_[size - 4];
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
}
