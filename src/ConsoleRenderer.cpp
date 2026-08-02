#include "octozone/ConsoleRenderer.hpp"
#include "octozone/VisionSystem.hpp"

#include <cstdlib>
#include <iostream>

namespace octozone
{
    void ConsoleRenderer::clear()
    {
        std::system("cls");
    }

    void ConsoleRenderer::draw(
        const Grid& grid,
        const Octopus& octopus,
        const std::vector<Shark>& sharks)
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

        printDebugInfo(octopus);
    }

    void ConsoleRenderer::drawResult(bool playerWon, bool timedOut)
    {
        if (timedOut)
        {
            std::cout << "Octopus ran out of time! Game over.\n";
        }
        else if (playerWon)
        {
            std::cout << "Octopus escaped! You win!\n";
        }
        else
        {
            std::cout << "Octopus caught! Game over.\n";
        }
    }

    void ConsoleRenderer::printDebugInfo(const Octopus& octopus) const
    {
        std::cout
            << "Decision: "
            << decisionToText(octopus.getDecision())
            << " | Objective: ";

        if (octopus.getDecision() == OctopusDecision::Hide &&
            octopus.getHideTarget().has_value())
        {
            Position hideTarget = octopus.getHideTarget().value();
            std::cout
                << "Hide at ("
                << hideTarget.row
                << ", "
                << hideTarget.col
                << ")";
        }
        else if (octopus.getDecision() == OctopusDecision::MoveToGoal)
        {
            Position goal = octopus.getGoal();
            std::cout
                << "Goal ("
                << goal.row
                << ", "
                << goal.col
                << ")";
        }
        else
        {
            std::cout << "Wait";
        }

        std::cout << '\n';
    }

    const char* ConsoleRenderer::decisionToText(
        OctopusDecision decision) const
    {
        switch (decision)
        {
            case OctopusDecision::MoveToGoal: return "MoveToGoal";
            case OctopusDecision::Hide: return "Hide";
            case OctopusDecision::Wait: return "Wait";
        }

        return "Unknown";
    }

    void ConsoleRenderer::printTile(Tile tile) const
    {
        switch (tile)
        {
            case Tile::Empty:
                std::cout << ". ";
                break;

            case Tile::Wall:
                std::cout << "# ";
                break;

            case Tile::Start:
                std::cout << "S ";
                break;

            case Tile::Goal:
                std::cout << "\033[34mG\033[0m ";
                break;

            case Tile::Seaweed:
                std::cout << "\033[32m~\033[0m ";
                break;
        }
    }

}
