# OctoZone

OctoZone is a C++20 autonomous stealth/pathfinding simulation. An AI-controlled octopus navigates an ocean grid, uses seaweed for stealth, avoids shark patrols and chase behavior, and tries to reach the goal using A* pathfinding with Manhattan movement.

The current version is not player-controlled. It is a simulation of competing AI systems: octopus decision-making versus shark perception, chase, search, and patrol behavior.

## Build

```powershell
cmake -S . -B build
cmake --build build --config Debug
.\build\Debug\OctoZone.exe
```

## Terrain Legend

- `.` empty water
- `#` wall
- `~` seaweed
- `O` octopus
- `X` shark
- `G` goal
- red `.` shark vision

`Grid` stores terrain only. Dynamic entities such as the octopus and sharks are rendered from entity state, not stored as terrain tiles. This keeps collision, AI, and future graphics rendering from mutating the map data.

## Architecture

```text
Game
|-- Grid
|-- MapGenerator
|-- Octopus
|-- OctopusBrain
|-- Shark
|-- SharkBrain
|-- VisionSystem
|-- Pathfinder
`-- IRenderer
    `-- ConsoleRenderer
```

`Game` coordinates interactions and turn resolution. Entities own persistent state. Brains make decisions. Systems such as pathfinding and vision stay independent of specific entity ownership. Renderers draw snapshots and debug information without changing gameplay state.

This separation is intentional: SFML can replace `ConsoleRenderer` later without moving AI, collision, map generation, or turn rules into the graphics layer.

## Turn Order

Each turn is resolved in a fixed order:

1. Sharks acquire visible targets from the rendered world state.
2. The octopus chooses and executes one move.
3. Immediate collision is resolved.
4. Sharks update awareness/state and move.
5. Collision and position-swap capture are resolved.
6. The goal condition is resolved after capture checks.

If the simulation exceeds the configured max turn count, the run times out and counts as a loss.

## End States

The game uses `GameResult` instead of paired booleans:

- `Running`
- `OctopusEscaped`
- `OctopusCaught`
- `TimedOut`

This avoids invalid states such as "not game over, but player won" and gives the future renderer a single value for end screens.

## Collision Rules

Actual capture is deliberately narrow:

- same tile means capture
- crossing positions in the same update means capture
- adjacency alone is not capture

Danger prediction may treat adjacency as risky, but danger is not the same as actual capture. Keeping `danger`, `potential capture`, and `actual capture` separate makes AI tuning safer.

## Octopus AI

`Octopus` owns position, goal, path, decision, hide target, and recent movement history.

`OctopusBrain` owns decision-making:

- danger assessment
- seaweed selection
- tactical move scoring
- loop avoidance
- final survival validation

The octopus does not have perfect global shark knowledge. `OctopusMemory` and `PerceptionSystem` provide limited perception, recent shark memory, and predicted positions for remembered sharks.

## Shark AI

`Shark` owns position, direction, patrol route, current state, last known octopus position, and search timer.

`SharkBrain` owns state transitions and movement decisions.

Shark states:

- `Patrol`: follow a patrol route
- `Chase`: pursue the octopus after seeing it
- `Search`: move toward the last known target and wait briefly
- `ReturnToPatrol`: traverse back to the patrol route

Seaweed is the primary stealth mechanic. Sharks can acquire chase through line-of-sight, and hiding in seaweed breaks detection.

## Pathfinding And Vision Rules

- Pathfinding uses four-way Manhattan movement only.
- No diagonal movement is allowed.
- Walls always block movement.
- Returned paths exclude the starting tile.
- Unreachable targets return an empty path.
- A `start == goal` path is empty because no movement is needed.
- Temporary blocked positions are respected, except seaweed remains enterable so the octopus can hide.
- Shark vision is forward-only for the requested distance.
- Walls and seaweed block vision rays.
- An octopus in seaweed is hidden.
- A shark with direction `{0, 0}` sees nothing until it moves.

A* accepts blocked danger positions because AI planning needs to reason about temporary threats separately from permanent terrain. This allows the same pathfinder to support goal movement, escape routing, patrol return, and shark collision avoidance.

## Map Generation

Map generation uses `std::mt19937` and supports deterministic seeds:

```cpp
GeneratedMap map = MapGenerator::generate(25, 25, 12345);
```

The default overload chooses a random seed and stores it in `GeneratedMap::seed` for debugging. This makes broken maps reproducible.

Generation is bounded by a max attempt count and validates:

- octopus and goal are distinct
- sharks do not spawn on the octopus or goal
- no entity starts inside a wall
- patrol routes have meaningful movement
- patrol routes stay in bounds and avoid walls, seaweed, and goal
- a route to the goal exists
- at least one seaweed tile is reachable

## Debug Telemetry

Rendering receives a `DebugInfo` snapshot with:

- octopus decision
- octopus objective
- octopus path length
- shark state
- shark last known target
- shark search turns remaining
- current map seed
- current turn and max turn count

This is currently shown in the console and can later become an SFML debug overlay.

## Current Limitations

- The simulation is autonomous; there is no player input yet.
- There are no automated unit tests yet for pathfinding, vision, or map generation.
- Console rendering is frame-delayed and not suited for real-time input.
- The octopus AI is heuristic-based and may still need tuning for edge-case maps.
- SFML rendering has not been added yet.
