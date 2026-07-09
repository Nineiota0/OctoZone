#include "octozone/Pathfinder.hpp"
#include <optional>

namespace octozone
{
    namespace
    {
        struct Node
        {
            Position position;
            int gCost{};
            int hCost{};
            std::optional<Position> parent;
        
            int fCost() const
            {
                return gCost + hCost;
            }
        };
    }
}