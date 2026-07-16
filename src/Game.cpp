#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#include "octozone/Game.hpp"
#include "octozone/MapGenerator.hpp"

namespace octozone
{
    Game::Game()
        : grid_(25, 25),
          renderer_(),
          octopus_({24, 0}, {0, 24})
    {
        GeneratedMap generatedMap = MapGenerator::generate(25, 25);

        grid_ = generatedMap.grid;
        octopus_ = Octopus(generatedMap.octopusStart, generatedMap.goal);

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
        while (!gameOver_)
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
        updateOctopus();
        updateSharks();
    }

    void Game::updateOctopus()
    {
        if (!sharks_.empty())
        {
            octopusBrain_.update(octopus_, sharks_.front(), grid_);
        }
    }

    void Game::updateSharks()
    {
        for (Shark& shark : sharks_)
        {
            shark.update(
                grid_,
                octopus_.getPosition(),
                octopus_.isHidden(grid_));
        }
    }

    void Game::render()
    {
        renderer_.draw(grid_, octopus_, sharks_);
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
        for (Shark& shark : sharks_)
        {
            if (shark.canDetect(grid_, octopus_.getPosition()))
            {
                shark.setState(SharkState::Chase);
            }

            if (shark.getPosition() == octopus_.getPosition())
            {
                gameOver_ = true;
                playerWon_ = false;
                return;
            }
        }
    }
}