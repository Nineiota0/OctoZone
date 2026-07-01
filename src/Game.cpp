#include "octozone/Game.hpp"

namespace octozone 
{

    Game::Game()
        : grid_(10, 10),
          renderer_()
    {
        initializeMap();
    }
    
    void Game::run()
    {
        renderer_.draw(grid_);
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