#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#include "octozone/Game.hpp"
#include "octozone/MapGenerator.hpp"
#include "octozone/Pathfinder.hpp"
#include "octozone/VisionSystem.hpp"
#include "octozone/OctopusDecision.hpp"

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
        updateShark();
    }

    void Game::updateOctopus()
    {
        Path danger = VisionSystem::getVisiblePositions(
            grid_,
            shark_.getPosition(),
            shark_.getDirection(),
            3
        );
    
        danger.push_back(shark_.getPosition());
    
        if (octopus_.getDecision() == OctopusDecision::Hide &&
            octopus_.getHideTarget().has_value())
        {
            Position hideTarget = octopus_.getHideTarget().value();
        
            if (octopus_.getPosition() == hideTarget)
            {
                octopus_.setDecision(OctopusDecision::Wait);
                return;
            }
        
            Path pathToHideTarget = Pathfinder::findPath(
                grid_,
                octopus_.getPosition(),
                hideTarget,
                danger
            );
        
            if (!pathToHideTarget.empty())
            {
                octopus_.setPath(pathToHideTarget);
                octopus_.moveOneStep();
                return;
            }
        
            octopus_.setDecision(OctopusDecision::Wait);
            return;
        }
    
        Path safePathToGoal = Pathfinder::findPath(
            grid_,
            octopus_.getPosition(),
            octopus_.getGoal(),
            danger
        );
    
        if (!safePathToGoal.empty())
        {
            octopus_.clearHideTarget();
            octopus_.setDecision(OctopusDecision::MoveToGoal);
            octopus_.setPath(safePathToGoal);
            octopus_.moveOneStep();
            return;
        }
    
        Path safePathToSeaweed = findPathToNearestSeaweed(danger);
    
        if (!safePathToSeaweed.empty())
        {
            Position hideTarget = safePathToSeaweed.back();
        
            octopus_.setHideTarget(hideTarget);
            octopus_.setDecision(OctopusDecision::Hide);
            octopus_.setPath(safePathToSeaweed);
            octopus_.moveOneStep();
            return;
        }
    
        octopus_.setDecision(OctopusDecision::Wait);
    }

    void Game::updateShark()
    {
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

    Path Game::findPathToNearestSeaweed(const Path& danger) const
    {
        Path bestPath;

        for (int row = 0; row < grid_.getRows(); ++row)
        {
            for (int col = 0; col < grid_.getCols(); ++col)
            {
                Position position{row, col};

                if (grid_.getTile(position) != Tile::Seaweed)
                {
                    continue;
                }

                Path path = Pathfinder::findPath(
                    grid_,
                    octopus_.getPosition(),
                    position,
                    danger
                );

                if (!path.empty() &&
                    (bestPath.empty() || path.size() < bestPath.size()))
                {
                    bestPath = path;
                }
            }
        }

        return bestPath;
    }
}