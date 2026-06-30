#include <iostream>
#include "octozone/Grid.hpp"

int main() 
{
    octozone::Grid grid(10, 10);

    grid.setTile({9, 0}, octozone::Tile::Octopus);
    grid.setTile({0, 9}, octozone::Tile::Goal);

    grid.setTile({4, 3}, octozone::Tile::Wall);
    grid.setTile({4, 4}, octozone::Tile::Wall);
    grid.setTile({4, 5}, octozone::Tile::Wall);

    grid.print();

    return 0;
}