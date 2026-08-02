#pragma once

#include "octozone/Grid.hpp"
#include "octozone/Path.hpp"
#include "octozone/Position.hpp"

#include <random>
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
        static GeneratedMap generate(int rows, int cols, unsigned int seed);

    private:
        static Position randomPosition(
            int rows,
            int cols,
            std::mt19937& rng);
        static Position randomEdgePosition(
            int rows,
            int cols,
            std::mt19937& rng);
        static int manhattanDistance(const Position& a, const Position& b);

        static Path createSharkPatrolRoute(
            const Grid& grid,
            const Position& sharkStart,
            std::mt19937& rng);

        static bool canUsePatrolPosition(
            const Grid& grid,
            const Position& position);

        static bool appendPatrolStep(
            const Grid& grid,
            Path& route,
            const Position& direction);

        static bool appendPatrolLeg(
            const Grid& grid,
            Path& route,
            const Position& direction,
            int length);

        static bool isValidGeneratedMap(
            const GeneratedMap& map);

        static bool containsPosition(
            const std::vector<GeneratedShark>& sharks,
            const Position& position);
    };
}
