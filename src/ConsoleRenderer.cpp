#include "octozone/ConsoleRenderer.hpp"
#include "octozone/VisionSystem.hpp"

#include <iostream>

namespace octozone
{

    void ConsoleRenderer::draw(
        const Grid& grid,
        const Octopus& octopus,
        const std::vector<Shark>& sharks) const
    {
        Path visiblePositions;

        for (const Shark& shark : sharks)
        {
            Path sharkVisiblePositions = VisionSystem::getVisiblePositions(
                grid,
                shark.getPosition(),
                shark.getDirection(),
                3
            );

            visiblePositions.insert(
                visiblePositions.end(),
                sharkVisiblePositions.begin(),
                sharkVisiblePositions.end()
            );
        }

        for (int row = 0; row < grid.getRows(); ++row)
        {
            for (int col = 0; col < grid.getCols(); ++col)
            {
                Position position{row, col};

                if (position == octopus.getPosition())
                {
                    std::cout << "\033[34mO\033[0m ";
                    continue;
                }

                bool sharkHere = false;

                for (const Shark& shark : sharks)
                {
                    if (position == shark.getPosition())
                    {
                        sharkHere = true;
                        break;
                    }
                }

                if (sharkHere)
                {
                    std::cout << "\033[31mX\033[0m ";
                    continue;
                }

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
                    continue;
                }

                printTile(grid.getTile(position));
            }

            std::cout << '\n';
        }
    }

    void ConsoleRenderer::printTile(Tile tile) const
    {
        switch (tile)
        {
            case Tile::Goal:
                std::cout << "\033[34mG\033[0m ";
                break;

            case Tile::Seaweed:
                std::cout << "\033[32m~\033[0m ";
                break;

            default:
                std::cout << tileToChar(tile) << ' ';
                break;
        }
    }

    char ConsoleRenderer::tileToChar(Tile tile) const
    {
        switch (tile)
        {
            case Tile::Empty: return '.';
            case Tile::Wall: return '#';
            case Tile::Start: return 'S';
            case Tile::Goal: return 'G';
            case Tile::Octopus: return 'O';
            case Tile::Shark: return 'X';
            case Tile::Seaweed: return '~';
        }

        return '?';
    }

}