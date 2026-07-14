#include "octozone/Shark.hpp"

namespace octozone
{

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

}