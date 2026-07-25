#include "octozone/MapGenerator.hpp"
#include "octozone/Pathfinder.hpp"

#include <cstdlib>
#include <ctime>

namespace octozone
{
    GeneratedMap MapGenerator::generate(int rows, int cols)
    {
        static bool seeded = false;

        if (!seeded)
        {
            std::srand(static_cast<unsigned int>(std::time(nullptr)));
            seeded = true;
        }

        while (true)
        {
            Grid grid(rows, cols);

            Position goal = randomEdgePosition(rows, cols);
            Position octopusStart = randomEdgePosition(rows, cols);

            int minimumStartDistance = 12;

            while (octopusStart == goal ||
                   manhattanDistance(octopusStart, goal) < minimumStartDistance)
            {
                octopusStart = randomEdgePosition(rows, cols);
            }

            int wallCount = 75;

            for (int i = 0; i < wallCount; ++i)
            {
                Position wallPosition = randomPosition(rows, cols);

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
                Position seaweedPosition = randomPosition(rows, cols);

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
                Position sharkStart = randomPosition(rows, cols);

                while (sharkStart == octopusStart ||
                       sharkStart == goal ||
                       grid.getTile(sharkStart) == Tile::Wall ||
                       grid.getTile(sharkStart) == Tile::Seaweed ||
                       containsPosition(sharks, sharkStart))
                {
                    sharkStart = randomPosition(rows, cols);
                }

                Path sharkPatrolRoute = createSharkPatrolRoute(grid, sharkStart);

                sharks.push_back({
                    sharkStart,
                    sharkPatrolRoute
                });
            }

            Path path = Pathfinder::findPath(
                grid,
                octopusStart,
                goal
            );

            if (!path.empty())
            {
                return GeneratedMap{
                    grid,
                    octopusStart,
                    goal,
                    sharks
                };
            }
        }
    }

    Position MapGenerator::randomPosition(int rows, int cols)
    {
        return Position{
            std::rand() % rows,
            std::rand() % cols
        };
    }

    Position MapGenerator::randomEdgePosition(int rows, int cols)
    {
        int edge = std::rand() % 4;

        switch (edge)
        {
            case 0:
                return Position{0, std::rand() % cols};

            case 1:
                return Position{rows - 1, std::rand() % cols};

            case 2:
                return Position{std::rand() % rows, 0};

            case 3:
                return Position{std::rand() % rows, cols - 1};
        }

        return Position{0, 0};
    }

    int MapGenerator::manhattanDistance(const Position& a, const Position& b)
    {
        return std::abs(a.row - b.row) + std::abs(a.col - b.col);
    }

    Path MapGenerator::createSharkPatrolRoute(
        const Grid& grid,
        const Position& sharkStart)
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

            int pattern = std::rand() % 4;

            if (pattern == 0)
            {
                appendPatrolLeg(
                    grid,
                    route,
                    directions[std::rand() % 4],
                    2 + std::rand() % 5);
            }
            else if (pattern == 1)
            {
                Position firstDirection = directions[std::rand() % 4];
                Position secondDirection = firstDirection.row == 0
                    ? directions[std::rand() % 2]
                    : directions[2 + std::rand() % 2];

                appendPatrolLeg(
                    grid,
                    route,
                    firstDirection,
                    1 + std::rand() % 4);

                appendPatrolLeg(
                    grid,
                    route,
                    secondDirection,
                    1 + std::rand() % 4);
            }
            else if (pattern == 2)
            {
                Position firstDirection = directions[std::rand() % 4];
                Position secondDirection = firstDirection.row == 0
                    ? directions[std::rand() % 2]
                    : directions[2 + std::rand() % 2];

                appendPatrolLeg(grid, route, firstDirection, 1 + std::rand() % 3);
                appendPatrolLeg(grid, route, secondDirection, 1 + std::rand() % 3);
                appendPatrolLeg(grid, route, {-firstDirection.row, -firstDirection.col}, 1 + std::rand() % 3);
            }
            else
            {
                int stepCount = 4 + std::rand() % 5;

                for (int step = 0; step < stepCount; ++step)
                {
                    Position direction = directions[std::rand() % 4];
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
