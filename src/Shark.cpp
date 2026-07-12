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
    }

}