#include "octozone/Path.hpp"

#include <iostream>

int main()
{
    octozone::Path path;

    path.push_back({9, 0});
    path.push_back({8, 0});
    path.push_back({7, 0});

    for (const auto& position : path)
    {
        std::cout << "("
                  << position.row
                  << ", "
                  << position.col
                  << ")\n";
    }

    return 0;
}