#include "octozone/ConsoleRenderer.hpp"
#include "octozone/VisionSystem.hpp"

#include <iostream>

namespace octozone
{

    void ConsoleRenderer::draw(
        const Grid& grid,
        const Octopus& octopus,
        const Shark& shark) const
    {
        Path visiblePositions = VisionSystem::getVisiblePositions(
            grid,
            shark.getPosition(),
            shark.getDirection(),
            3
        );

        for (int row = 0; row < grid.getRows(); ++row)
        {
            for (int col = 0; col < grid.getCols(); ++col)
            {
                Position position{row, col};

                if (position == octopus.getPosition())
                {
                    std::cout << 'O' << ' ';
                }
                else if (position == shark.getPosition())
                {
                    std::cout << 'X' << ' ';
                }
                else
                {
                    bool inFov = false;

                    for (const Position& visiblePosition : visiblePositions)
                    {
                        if (position == visiblePosition)
                        {
                            inFov = true;
                            break;
                        }
                    }

                    if (inFov)
                    {
                        std::cout << "\033[31m.\033[0m ";
                    }
                    else
                    {
                        std::cout << tileToChar(grid.getTile(position)) << ' ';
                    }
                }
            }

            std::cout << '\n';
        }
    }

    char ConsoleRenderer::tileToChar(Tile tile) const
    {
        switch (tile)
        {
            case Tile::Empty: return '.';
            case Tile::Wall:  return '#';
            case Tile::Start: return 'S';
            case Tile::Goal:  return 'G';
            case Tile::Octopus: return 'O';
            case Tile::Shark: return 'X';
        }

        return '?';
    }

}