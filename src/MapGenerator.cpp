#include "octozone/MapGenerator.hpp"

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

        Grid grid(rows, cols);

        Position octopusStart = randomPosition(rows, cols);
        Position goal = randomPosition(rows, cols);

        while (goal == octopusStart)
        {
            goal = randomPosition(rows, cols);
        }

        int wallCount = 15;

        for (int i = 0; i < wallCount; ++i)
        {
            Position wallPosition = randomPosition(rows, cols);

            if (wallPosition != octopusStart && wallPosition != goal)
            {
                grid.setTile(wallPosition, Tile::Wall);
            }
        }

        Position sharkStart{2, 6};

        Path sharkPatrolRoute{
            sharkStart,
            Position{2, 7},
            Position{2, 8},
            Position{2, 7}
        };

        grid.setTile(goal, Tile::Goal);

        return GeneratedMap{
            grid,
            octopusStart,
            goal,
            sharkStart,
            sharkPatrolRoute
        };
    }

    Position MapGenerator::randomPosition(int rows, int cols)
    {
        return Position{
            std::rand() % rows,
            std::rand() % cols
        };
    }
}