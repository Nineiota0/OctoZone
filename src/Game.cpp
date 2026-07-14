#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#include "octozone/Game.hpp"
#include "octozone/MapGenerator.hpp"
#include "octozone/Pathfinder.hpp"
#include "octozone/VisionSystem.hpp"

namespace octozone
{
    Game::Game()
        : grid_(10, 10),
          renderer_(),
          octopus_({9, 0}, {0, 9}),
          shark_({2, 6}, Path{{2, 6}, {2, 7}, {2, 8}, {2, 7}})
    {
        GeneratedMap generatedMap = MapGenerator::generate(10, 10);

        grid_ = generatedMap.grid;
        octopus_ = Octopus(generatedMap.octopusStart, generatedMap.goal);
        shark_ = Shark(generatedMap.sharkStart, generatedMap.sharkPatrolRoute);

        Path path = Pathfinder::findPath(
            grid_,
            octopus_.getPosition(),
            octopus_.getGoal()
        );

        octopus_.setPath(path);
    }

    void Game::run()
    {
        while (!gameOver_ && octopus_.hasPath())
        {
            std::system("cls");

            render();
            update();

            checkLoseCondition();
            checkWinCondition();

            std::this_thread::sleep_for(std::chrono::milliseconds(400));
        }

        std::system("cls");
        render();

        if (playerWon_)
        {
            std::cout << "Octopus escaped! You win!\n";
        }
        else
        {
            std::cout << "Octopus caught! Game over.\n";
        }
    }

    void Game::update()
    {
        octopus_.moveOneStep();

        if (shark_.isChasing())
        {
            Path chasePath = Pathfinder::findPath(
                grid_,
                shark_.getPosition(),
                octopus_.getPosition()
            );

            if (!chasePath.empty())
            {
                shark_.moveTo(chasePath.front());
            }
        }
        else
        {
            shark_.moveOneStep();
        }
    }

    void Game::render()
    {
        renderer_.draw(grid_, octopus_, shark_);
        std::cout << '\n';
    }

    void Game::initializeMap()
    {
        // Map generation now handles terrain setup.
    }

    void Game::checkWinCondition()
    {
        if (octopus_.getPosition() == octopus_.getGoal())
        {
            gameOver_ = true;
            playerWon_ = true;
        }
    }

    void Game::checkLoseCondition()
    {
        if (VisionSystem::canDetect(
                grid_,
                shark_.getPosition(),
                shark_.getDirection(),
                octopus_.getPosition(),
                3))
        {
            shark_.setState(SharkState::Chase);
        }

        if (shark_.getPosition() == octopus_.getPosition())
        {
            gameOver_ = true;
            playerWon_ = false;
        }
    }
}