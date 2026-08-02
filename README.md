# OctoZone

OctoZone is a C++20 stealth/pathfinding simulation where an autonomous octopus navigates an ocean grid, avoids shark vision cones, and finds the shortest path to the exit using A* pathfinding.

The octopus is AI-controlled; the current game is a fully autonomous simulation, not a player-controlled game.

## Gameplay Loop

Each turn is resolved in a fixed order:

1. Sharks acquire visible targets from the rendered world state.
2. The octopus chooses and executes one move.
3. Immediate collision is resolved.
4. Sharks update awareness/state and move.
5. Collision and position-swap capture are resolved.
6. The goal condition is resolved after capture checks.

If the simulation exceeds the configured max turn count, the run times out and counts as a loss.

## Pathfinding And Vision Rules

- Pathfinding uses four-way Manhattan movement only.
- Walls always block movement.
- Returned paths exclude the starting tile.
- Unreachable targets return an empty path.
- A `start == goal` path is empty because no movement is needed.
- Temporary blocked positions are respected, except seaweed remains enterable so the octopus can hide.
- Shark vision is forward-only for the requested distance.
- Walls and seaweed block vision rays.
- An octopus in seaweed is hidden.
- A shark with direction `{0, 0}` sees nothing until it moves.

## Current Status

- C++20 project setup
- CMake build system
- GitHub repository connected

## Planned Features

- 2D grid map
- Obstacles
- A* pathfinding
- Shark patrol behavior
- Vision cone detection
- Win/lose conditions
