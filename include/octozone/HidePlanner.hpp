#pragma once

#include "octozone/Grid.hpp"
#include "octozone/Octopus.hpp"
#include "octozone/Path.hpp"
#include "octozone/Shark.hpp"

namespace octozone
{
    class HidePlanner
    {
    public:
        Path findPathToBestSeaweed(
            const Grid& grid,
            const Octopus& octopus,
            const Shark& shark,
            const Path& danger) const;

    private:
        int manhattanDistance(const Position& a, const Position& b) const;
    };
}