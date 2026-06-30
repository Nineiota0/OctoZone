#include "octozone/ConsoleRenderer.hpp"
#include "octozone/Grid.hpp"

#include <iostream>
#include <stdexcept>

using namespace octozone;

int main()
{
    Grid grid(5, 5);

    // Test constructor + getters
    std::cout << "Rows: " << grid.getRows() << '\n';
    std::cout << "Cols: " << grid.getCols() << '\n';

    // Test isInBounds()
    std::cout << "\nTesting isInBounds()\n";

    std::cout << "(0,0): "
              << grid.isInBounds({0, 0}) << '\n';

    std::cout << "(4,4): "
              << grid.isInBounds({4, 4}) << '\n';

    std::cout << "(5,5): "
              << grid.isInBounds({5, 5}) << '\n';

    std::cout << "(-1,0): "
              << grid.isInBounds({-1, 0}) << '\n';

    // Test setTile() and getTile()
    std::cout << "\nTesting setTile() and getTile()\n";

    grid.setTile({2, 3}, Tile::Wall);

    if (grid.getTile({2, 3}) == Tile::Wall)
    {
        std::cout << "Wall placed successfully!\n";
    }

    // Test exception
    std::cout << "\nTesting exception...\n";

    try
    {
        grid.getTile({10, 10});
    }
    catch (const std::out_of_range& e)
    {
        std::cout << "Caught exception: "
                  << e.what()
                  << '\n';
    }

    return 0;
}