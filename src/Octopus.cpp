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

}