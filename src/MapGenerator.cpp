#include "octozone/MapGenerator.hpp"
#include "octozone/Pathfinder.hpp"

#include <cstdlib>
#include <random>
#include <stdexcept>

namespace octozone
{
    GeneratedMap MapGenerator::generate(int rows, int cols)
    {
        std::random_device randomDevice;
        return generate(rows, cols, randomDevice());
    }

    GeneratedMap MapGenerator::generate(int rows, int cols, unsigned int seed)
    {
        constexpr int maxGenerationAttempts = 1000;
        std::mt19937 rng(seed);

        for (int attempt = 0; attempt < maxGenerationAttempts; ++attempt)
        {
            Grid grid(rows, cols);

            Position goal = randomEdgePosition(rows, cols, rng);
            Position octopusStart = randomEdgePosition(rows, cols, rng);

            int minimumStartDistance = 12;
            int startAttempts = 0;

            while (octopusStart == goal ||
                   manhattanDistance(octopusStart, goal) < minimumStartDistance)
            {
                octopusStart = randomEdgePosition(rows, cols, rng);

                if (++startAttempts > maxGenerationAttempts)
                {
                    break;
                }
            }

            int wallCount = 75;

            for (int i = 0; i < wallCount; ++i)
            {
                Position wallPosition = randomPosition(rows, cols, rng);

                if (wallPosition != octopusStart &&
                    wallPosition != goal &&
                    grid.getTile(wallPosition) == Tile::Empty)
                {
                    grid.setTile(wallPosition, Tile::Wall);
                }
            }

            int seaweedCount = 15;

            for (int i = 0; i < seaweedCount; ++i)
            {
                Position seaweedPosition = randomPosition(rows, cols, rng);

                if (seaweedPosition != octopusStart &&
                    seaweedPosition != goal &&
                    grid.getTile(seaweedPosition) == Tile::Empty)
                {
                    grid.setTile(seaweedPosition, Tile::Seaweed);
                }
            }

            grid.setTile(goal, Tile::Goal);

            std::vector<GeneratedShark> sharks;
            int sharkCount = 5;

            for (int i = 0; i < sharkCount; ++i)
            {
                Position sharkStart = randomPosition(rows, cols, rng);
                int sharkStartAttempts = 0;

                while (sharkStart == octopusStart ||
                       sharkStart == goal ||
                       grid.getTile(sharkStart) == Tile::Wall ||
                       grid.getTile(sharkStart) == Tile::Seaweed ||
                       containsPosition(sharks, sharkStart))
                {
                    sharkStart = randomPosition(rows, cols, rng);

                    if (++sharkStartAttempts > maxGenerationAttempts)
                    {
                        break;
                    }
                }

                Path sharkPatrolRoute = createSharkPatrolRoute(
                    grid,
                    sharkStart,
                    rng);

                sharks.push_back({
                    sharkStart,
                    sharkPatrolRoute
                });
            }

            GeneratedMap map{
                grid,
                octopusStart,
                goal,
                sharks,
                seed
            };

            if (isValidGeneratedMap(map))
            {
                return map;
            }
        }

        throw std::runtime_error("Failed to generate a valid map.");
    }

    Position MapGenerator::randomPosition(
        int rows,
        int cols,
        std::mt19937& rng)
    {
        std::uniform_int_distribution<int> rowDistribution(0, rows - 1);
        std::uniform_int_distribution<int> colDistribution(0, cols - 1);

        return Position{
            rowDistribution(rng),
            colDistribution(rng)
        };
    }

    Position MapGenerator::randomEdgePosition(
        int rows,
        int cols,
        std::mt19937& rng)
    {
        std::uniform_int_distribution<int> edgeDistribution(0, 3);
        std::uniform_int_distribution<int> rowDistribution(0, rows - 1);
        std::uniform_int_distribution<int> colDistribution(0, cols - 1);

        int edge = edgeDistribution(rng);

        switch (edge)
        {
            case 0:
                return Position{0, colDistribution(rng)};

            case 1:
                return Position{rows - 1, colDistribution(rng)};

            case 2:
                return Position{rowDistribution(rng), 0};

            case 3:
                return Position{rowDistribution(rng), cols - 1};
        }

        return Position{0, 0};
    }

    int MapGenerator::manhattanDistance(const Position& a, const Position& b)
    {
        return std::abs(a.row - b.row) + std::abs(a.col - b.col);
    }

    Path MapGenerator::createSharkPatrolRoute(
        const Grid& grid,
        const Position& sharkStart,
        std::mt19937& rng)
    {
        const Position directions[] = {
            {-1, 0},
            {1, 0},
            {0, -1},
            {0, 1}
        };

        for (int attempt = 0; attempt < 30; ++attempt)
        {
            Path route;
            route.push_back(sharkStart);

            std::uniform_int_distribution<int> directionDistribution(0, 3);
            std::uniform_int_distribution<int> patternDistribution(0, 3);
            std::uniform_int_distribution<int> shortLegDistribution(1, 3);
            std::uniform_int_distribution<int> legDistribution(1, 4);
            std::uniform_int_distribution<int> straightLegDistribution(2, 6);
            std::uniform_int_distribution<int> randomWalkDistribution(4, 8);

            int pattern = patternDistribution(rng);

            if (pattern == 0)
            {
                appendPatrolLeg(
                    grid,
                    route,
                    directions[directionDistribution(rng)],
                    straightLegDistribution(rng));
            }
            else if (pattern == 1)
            {
                Position firstDirection = directions[directionDistribution(rng)];
                Position secondDirection = firstDirection.row == 0
                    ? directions[directionDistribution(rng) % 2]
                    : directions[2 + directionDistribution(rng) % 2];

                appendPatrolLeg(
                    grid,
                    route,
                    firstDirection,
                    legDistribution(rng));

                appendPatrolLeg(
                    grid,
                    route,
                    secondDirection,
                    legDistribution(rng));
            }
            else if (pattern == 2)
            {
                Position firstDirection = directions[directionDistribution(rng)];
                Position secondDirection = firstDirection.row == 0
                    ? directions[directionDistribution(rng) % 2]
                    : directions[2 + directionDistribution(rng) % 2];

                appendPatrolLeg(grid, route, firstDirection, shortLegDistribution(rng));
                appendPatrolLeg(grid, route, secondDirection, shortLegDistribution(rng));
                appendPatrolLeg(grid, route, {-firstDirection.row, -firstDirection.col}, shortLegDistribution(rng));
            }
            else
            {
                int stepCount = randomWalkDistribution(rng);

                for (int step = 0; step < stepCount; ++step)
                {
                    Position direction = directions[directionDistribution(rng)];
                    appendPatrolStep(grid, route, direction);
                }
            }

            if (route.size() > 1)
            {
                return route;
            }
        }

        return Path{sharkStart};
    }

    bool MapGenerator::canUsePatrolPosition(
        const Grid& grid,
        const Position& position)
    {
        return grid.isInBounds(position) &&
            grid.getTile(position) != Tile::Wall &&
            grid.getTile(position) != Tile::Goal &&
            grid.getTile(position) != Tile::Seaweed;
    }

    bool MapGenerator::appendPatrolStep(
        const Grid& grid,
        Path& route,
        const Position& direction)
    {
        Position current = route.back();
        Position next{
            current.row + direction.row,
            current.col + direction.col
        };

        if (!canUsePatrolPosition(grid, next))
        {
            return false;
        }

        route.push_back(next);
        return true;
    }

    bool MapGenerator::appendPatrolLeg(
        const Grid& grid,
        Path& route,
        const Position& direction,
        int length)
    {
        bool addedStep = false;

        for (int step = 0; step < length; ++step)
        {
            if (!appendPatrolStep(grid, route, direction))
            {
                break;
            }

            addedStep = true;
        }

        return addedStep;
    }

    bool MapGenerator::isValidGeneratedMap(const GeneratedMap& map)
    {
        if (map.octopusStart == map.goal ||
            !map.grid.isInBounds(map.octopusStart) ||
            !map.grid.isInBounds(map.goal) ||
            map.grid.getTile(map.octopusStart) == Tile::Wall ||
            map.grid.getTile(map.goal) != Tile::Goal)
        {
            return false;
        }

        if (Pathfinder::findPath(
                map.grid,
                map.octopusStart,
                map.goal).empty())
        {
            return false;
        }

        bool hasReachableSeaweed = false;

        for (int row = 0; row < map.grid.getRows(); ++row)
        {
            for (int col = 0; col < map.grid.getCols(); ++col)
            {
                Position position{row, col};

                if (map.grid.getTile(position) == Tile::Seaweed &&
                    !Pathfinder::findPath(
                        map.grid,
                        map.octopusStart,
                        position).empty())
                {
                    hasReachableSeaweed = true;
                }
            }
        }

        if (!hasReachableSeaweed)
        {
            return false;
        }

        for (const GeneratedShark& shark : map.sharks)
        {
            if (shark.start == map.octopusStart ||
                shark.start == map.goal ||
                !map.grid.isInBounds(shark.start) ||
                map.grid.getTile(shark.start) == Tile::Wall ||
                shark.patrolRoute.size() < 2)
            {
                return false;
            }

            for (Position patrolPosition : shark.patrolRoute)
            {
                if (!canUsePatrolPosition(map.grid, patrolPosition))
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool MapGenerator::containsPosition(
        const std::vector<GeneratedShark>& sharks,
        const Position& position)
    {
        for (const GeneratedShark& shark : sharks)
        {
            if (shark.start == position)
            {
                return true;
            }
        }

        return false;
    }
}
