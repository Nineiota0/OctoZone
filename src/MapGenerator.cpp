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
                    
            int minimumStartDistance = 6;
                    
            while (octopusStart == goal ||
                   manhattanDistance(octopusStart, goal) < minimumStartDistance)
            {
                octopusStart = randomEdgePosition(rows, cols);
            }

            int wallCount = 15;

            for (int i = 0; i < wallCount; ++i)
            {
                Position wallPosition = randomPosition(rows, cols);

                if (wallPosition != octopusStart &&
                    wallPosition != goal)
                {
                    grid.setTile(wallPosition, Tile::Wall);
                }
            }

            int seaweedCount = 8;

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

            Position sharkStart = randomPosition(rows, cols);
                    
            while (sharkStart == octopusStart ||
                   sharkStart == goal ||
                   grid.getTile(sharkStart) == Tile::Wall)
            {
                sharkStart = randomPosition(rows, cols);
            }
            
            Path sharkPatrolRoute = createSharkPatrolRoute(grid, sharkStart);

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
                    sharkStart,
                    sharkPatrolRoute
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
        Path route;
        route.push_back(sharkStart);

        const Position directions[] = {
            {-1, 0}, // up
            {1, 0},  // down
            {0, -1}, // left
            {0, 1}   // right
        };

        Position direction = directions[std::rand() % 4];

        Position current = sharkStart;

        for (int i = 0; i < 3; ++i)
        {
            Position next{
                current.row + direction.row,
                current.col + direction.col
            };

            if (!grid.isInBounds(next) ||
                grid.getTile(next) == Tile::Wall ||
                grid.getTile(next) == Tile::Goal)
            {
                break;
            }

            route.push_back(next);
            current = next;
        }

        if (route.size() == 1)
        {
            return createSharkPatrolRoute(grid, sharkStart);
        }

        return route;
    }
}
