#include <chrono>
#include <algorithm>
#include <memory>
#include <thread>

#include "octozone/ConsoleRenderer.hpp"
#include "octozone/Game.hpp"
#include "octozone/MapGenerator.hpp"

namespace octozone
{
    namespace
    {
        void removeFirstPosition(Path& positions, Position position)
        {
            auto found = std::find(
                positions.begin(),
                positions.end(),
                position);

            if (found != positions.end())
            {
                positions.erase(found);
            }
        }
    }

    Game::Game()
        : Game(std::make_unique<ConsoleRenderer>())
    {
    }

    Game::Game(std::unique_ptr<IRenderer> renderer)
        : grid_(25, 25),
          renderer_(std::move(renderer)),
          octopus_({24, 0}, {0, 24})
    {
        GeneratedMap generatedMap = MapGenerator::generate(25, 25);

        grid_ = generatedMap.grid;
        octopus_ = Octopus(generatedMap.octopusStart, generatedMap.goal);
        mapSeed_ = generatedMap.seed;

        for (const GeneratedShark& generatedShark : generatedMap.sharks)
        {
            sharks_.emplace_back(
                generatedShark.start,
                generatedShark.patrolRoute
            );
        }
    }

    void Game::run()
    {
        while (isRunning())
        {
            renderer_->clear();
            render();
            resolveTurn();

            std::this_thread::sleep_for(std::chrono::milliseconds(400));
        }

        renderer_->clear();
        render();
        renderer_->drawResult(result_);
    }

    void Game::update()
    {
        resolveTurn();
    }

    void Game::resolveTurn()
    {
        // Turn order:
        // 1. Sharks acquire visible targets from the rendered world state.
        // 2. Octopus moves.
        // 3. Immediate collision is resolved.
        // 4. Sharks update awareness/state and move.
        // 5. Collision, including position swaps, is resolved.
        // 6. Goal is resolved after capture checks.
        ++turnCount_;

        if (turnCount_ > maxTurns_)
        {
            result_ = GameResult::TimedOut;
            return;
        }

        refreshSharkChases();

        Position previousOctopusPosition = octopus_.getPosition();
        std::vector<Position> previousSharkPositions;

        for (const Shark& shark : sharks_)
        {
            previousSharkPositions.push_back(shark.getPosition());
        }

        updateOctopus();

        if (resolveCapture(previousOctopusPosition, previousSharkPositions, false))
        {
            return;
        }

        updateSharks();

        if (resolveCapture(previousOctopusPosition, previousSharkPositions, true))
        {
            return;
        }

        resolveGoal();
    }

    void Game::updateOctopus()
    {
        if (!sharks_.empty())
        {
            octopusBrain_.update(octopus_, sharks_, grid_);
        }
    }

    void Game::updateSharks()
    {
        Path occupiedPositions;

        for (const Shark& shark : sharks_)
        {
            occupiedPositions.push_back(shark.getPosition());
        }

        for (Shark& shark : sharks_)
        {
            removeFirstPosition(occupiedPositions, shark.getPosition());

            sharkBrain_.update(
                shark,
                grid_,
                octopus_.getPosition(),
                octopus_.isHidden(grid_),
                occupiedPositions);

            occupiedPositions.push_back(shark.getPosition());
        }
    }

    void Game::render()
    {
        renderer_->draw(grid_, octopus_, sharks_, buildDebugInfo());
    }

    DebugInfo Game::buildDebugInfo() const
    {
        DebugInfo debugInfo;
        debugInfo.octopusDecision = octopus_.getDecision();
        debugInfo.octopusPathLength = octopus_.getPathLength();
        debugInfo.mapSeed = mapSeed_;
        debugInfo.turnCount = turnCount_;
        debugInfo.maxTurns = maxTurns_;

        if (octopus_.getDecision() == OctopusDecision::Hide)
        {
            debugInfo.octopusObjective = octopus_.getHideTarget();
        }
        else if (octopus_.getDecision() == OctopusDecision::MoveToGoal)
        {
            debugInfo.octopusObjective = octopus_.getGoal();
        }

        for (const Shark& shark : sharks_)
        {
            debugInfo.sharks.push_back({
                shark.getState(),
                shark.getLastKnownOctopusPosition(),
                shark.getSearchTurnsRemaining()
            });
        }

        return debugInfo;
    }

    void Game::refreshSharkChases()
    {
        if (octopus_.isHidden(grid_))
        {
            return;
        }

        for (Shark& shark : sharks_)
        {
            if (shark.canDetect(grid_, octopus_.getPosition()))
            {
                shark.beginChase(octopus_.getPosition());
            }
        }
    }

    bool Game::resolveCapture(
        Position previousOctopusPosition,
        const std::vector<Position>& previousSharkPositions,
        bool includeSwaps)
    {
        refreshSharkChases();

        for (std::size_t index = 0; index < sharks_.size(); ++index)
        {
            const Shark& shark = sharks_[index];

            bool samePosition =
                shark.getPosition() == octopus_.getPosition();

            bool swappedPositions =
                includeSwaps &&
                index < previousSharkPositions.size() &&
                previousSharkPositions[index] == octopus_.getPosition() &&
                shark.getPosition() == previousOctopusPosition;

            if (samePosition || swappedPositions)
            {
                result_ = GameResult::OctopusCaught;
                return true;
            }
        }

        return false;
    }

    void Game::resolveGoal()
    {
        if (octopus_.getPosition() == octopus_.getGoal())
        {
            result_ = GameResult::OctopusEscaped;
        }
    }

    bool Game::isRunning() const
    {
        return result_ == GameResult::Running;
    }
}
