#include <iostream>

#include "octozone/Game.hpp"
#include "octozone/Pathfinder.hpp"

namespace octozone
{
    Game::Game()
        : grid_(10, 10),
          renderer_(),
          octopus_({9, 0}, {0, 9})
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
        render();  // Show the initial state.
    
        while (octopus_.hasPath())
        {
            update();
            render();
        }
    }

    void Game::update()
    {
        Position oldPosition = octopus_.getPosition();

        octopus_.moveOneStep();

        Position newPosition = octopus_.getPosition();

        grid_.setTile(oldPosition, Tile::Empty);
        grid_.setTile(newPosition, Tile::Octopus);
    }

    void Game::render()
    {
        renderer_.draw(grid_);
        std::cout << '\n';
    }

    void Game::initializeMap()
    {
        grid_.setTile({9, 0}, Tile::Octopus);
        grid_.setTile({0, 9}, Tile::Goal);

        grid_.setTile({4, 3}, Tile::Wall);
        grid_.setTile({4, 4}, Tile::Wall);
        grid_.setTile({4, 5}, Tile::Wall);

        grid_.setTile({2, 6}, Tile::Shark);
    }
}