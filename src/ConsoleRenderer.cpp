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
        const std::vector<Shark>& sharks,
        const DebugInfo& debugInfo)
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

        printDebugInfo(debugInfo);
    }

    void ConsoleRenderer::drawResult(GameResult result)
    {
        switch (result)
        {
            case GameResult::OctopusEscaped:
                std::cout << "Octopus escaped! You win!\n";
                break;

            case GameResult::OctopusCaught:
                std::cout << "Octopus caught! Game over.\n";
                break;

            case GameResult::TimedOut:
                std::cout << "Octopus ran out of time! Game over.\n";
                break;

            case GameResult::Running:
                break;
        }
    }

    void ConsoleRenderer::printDebugInfo(const DebugInfo& debugInfo) const
    {
        std::cout
            << "Seed: "
            << debugInfo.mapSeed
            << " | Turn: "
            << debugInfo.turnCount
            << "/"
            << debugInfo.maxTurns
            << " | "
            << "Decision: "
            << decisionToText(debugInfo.octopusDecision)
            << " | Objective: ";

        if (debugInfo.octopusObjective.has_value())
        {
            Position objective = debugInfo.octopusObjective.value();
            std::cout
                << "("
                << objective.row
                << ", "
                << objective.col
                << ")";
        }
        else
        {
            std::cout << "None";
        }

        std::cout
            << " | Path: "
            << debugInfo.octopusPathLength
            << '\n';

        for (std::size_t index = 0; index < debugInfo.sharks.size(); ++index)
        {
            const SharkDebugInfo& shark = debugInfo.sharks[index];

            std::cout
                << "Shark "
                << index
                << ": "
                << sharkStateToText(shark.state)
                << " | Last target: ";

            if (shark.lastKnownTarget.has_value())
            {
                Position target = shark.lastKnownTarget.value();
                std::cout << "(" << target.row << ", " << target.col << ")";
            }
            else
            {
                std::cout << "None";
            }

            std::cout
                << " | Search: "
                << shark.searchTurnsRemaining
                << '\n';
        }
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

    const char* ConsoleRenderer::sharkStateToText(SharkState state) const
    {
        switch (state)
        {
            case SharkState::Patrol: return "Patrol";
            case SharkState::Chase: return "Chase";
            case SharkState::Search: return "Search";
            case SharkState::ReturnToPatrol: return "ReturnToPatrol";
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
