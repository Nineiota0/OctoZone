#include "octozone/ConsoleRenderer.hpp"
#include "octozone/Grid.hpp"
#include "octozone/Pathfinder.hpp"

using namespace octozone;

int main()
{
    Grid grid(10, 10);

    Position start{9, 0};
    Position goal{0, 9};

    grid.setTile(start, Tile::Octopus);
    grid.setTile(goal, Tile::Goal);

    grid.setTile({4, 3}, Tile::Wall);
    grid.setTile({4, 4}, Tile::Wall);
    grid.setTile({4, 5}, Tile::Wall);

    Path path = Pathfinder::findPath(grid, start, goal);

    for (const Position& position : path)
    {
        if (position != start && position != goal)
        {
            grid.setTile(position, Tile::Start);
        }
    }

    ConsoleRenderer renderer;
    renderer.draw(grid);

    return 0;
}