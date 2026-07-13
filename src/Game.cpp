#include <iostream>
#include <chrono>
#include <cstdlib>
#include <thread>

#include "octozone/Game.hpp"
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
        initializeMap();

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
            std::system("cls"); // Windows clear screen
        
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
            std::cout << "Octopus detected! Game over.\n";
        }
    }

    void Game::update()
    {
        Position oldOctopusPosition = octopus_.getPosition();
    
        octopus_.moveOneStep();
    
        Position newOctopusPosition = octopus_.getPosition();
    
        grid_.setTile(oldOctopusPosition, Tile::Empty);
        grid_.setTile(newOctopusPosition, Tile::Octopus);
    
        Position oldSharkPosition = shark_.getPosition();
    
        shark_.moveOneStep();
    
        Position newSharkPosition = shark_.getPosition();
    
        if (oldSharkPosition != octopus_.getPosition())
        {
            grid_.setTile(oldSharkPosition, Tile::Empty);
        }
    
        grid_.setTile(newSharkPosition, Tile::Shark);
    
        if (VisionSystem::canDetect(
                grid_,
                shark_.getPosition(),
                octopus_.getPosition(),
                3))
        {
            std::cout << "Octopus detected! Game over.\n";
        }
    }

    void Game::render()
    {
        renderer_.draw(grid_);
        std::cout << '\n';
    }

    void Game::initializeMap()
    {
        grid_.setTile(octopus_.getPosition(), Tile::Octopus);
        grid_.setTile(octopus_.getGoal(), Tile::Goal);

        grid_.setTile({4, 3}, Tile::Wall);
        grid_.setTile({4, 4}, Tile::Wall);
        grid_.setTile({4, 5}, Tile::Wall);

        grid_.setTile(shark_.getPosition(), Tile::Shark);
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
                octopus_.getPosition(),
                3))
        {
            gameOver_ = true;
            playerWon_ = false;
        }
    }
}