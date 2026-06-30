#include "octozone/Grid.hpp"

#include <iostream>
#include <stdexcept>

namespace octozone 
{
    Grid::Grid(int rows, int cols)
        : rows_(rows),
          cols_(cols),
          tiles_(rows, std::vector<Tile>(cols, Tile::Empty)) {}
    
    int Grid::getRows() const 
    {
        return rows_;
    }
    
    int Grid::getCols() const 
    {
        return cols_;
    }
    
    bool Grid::isInBounds(Position position) const 
    {
        return position.row >= 0 &&
               position.row < rows_ &&
               position.col >= 0 &&
               position.col < cols_;
    }
    
    Tile Grid::getTile(Position position) const 
    {
        if (!isInBounds(position)) 
        {
            throw std::out_of_range("Position is outside the grid.");
        }
    
        return tiles_[position.row][position.col];
    }
    
    void Grid::setTile(Position position, Tile tile) 
    {
        if (!isInBounds(position)) 
        {
            throw std::out_of_range("Position is outside the grid.");
        }
    
        tiles_[position.row][position.col] = tile;
    }
    
    void Grid::print() const 
    {
        for (const auto& row : tiles_) 
        {
            for (Tile tile : row) {
                std::cout << tileToChar(tile) << ' ';
            }
            std::cout << '\n';
        }
    }
    
    char Grid::tileToChar(Tile tile) const 
    {
        switch (tile) 
        {
            case Tile::Empty: return '.';
            case Tile::Wall: return '#';
            case Tile::Start: return 'S';
            case Tile::Goal: return 'G';
            case Tile::Octopus: return 'O';
            case Tile::Shark: return 'X';
        }
    
        return '?';
    }

}