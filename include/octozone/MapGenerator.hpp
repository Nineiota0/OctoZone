#pragma once

#include "octozone/Grid.hpp"
#include "octozone/Path.hpp"
#include "octozone/Position.hpp"

#include <vector>

namespace octozone
{
    struct GeneratedShark
    {
        Position start;
        Path patrolRoute;
    };

    struct GeneratedMap
    {
        Grid grid;
        Position octopusStart;
        Position goal;
        std::vector<GeneratedShark> sharks;
    };

    class MapGenerator
    {
    public:
        static GeneratedMap generate(int rows, int cols);

    private:
        static Position randomPosition(int rows, int cols);
        static Position randomEdgePosition(int rows, int cols);
        static int manhattanDistance(const Position& a, const Position& b);

        static Path createSharkPatrolRoute(
            const Grid& grid,
            const Position& sharkStart);

        static bool containsPosition(
            const std::vector<GeneratedShark>& sharks,
            const Position& position);
    };
}