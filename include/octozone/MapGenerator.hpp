#pragma once

#include "octozone/Grid.hpp"
#include "octozone/Path.hpp"
#include "octozone/Position.hpp"

namespace octozone
{
    struct GeneratedMap
    {
        Grid grid;
        Position octopusStart;
        Position goal;
        Position sharkStart;
        Path sharkPatrolRoute;
    };

    class MapGenerator
    {
    public:
        static GeneratedMap generate(int rows, int cols);

    private:
        static Position randomPosition(int rows, int cols);
    };
}