cmake_minimum_required(VERSION 3.20)

project(OctoZone VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(OctoZone
    src/main.cpp
    src/Grid.cpp
    src/ConsoleRenderer.cpp
    src/Game.cpp
    src/Pathfinder.cpp
    src/Octopus.cpp
    src/Shark.cpp
    src/SharkBrain.cpp
    src/VisionSystem.cpp
    src/MapGenerator.cpp
    src/OctopusBrain.cpp
    src/OctopusMemory.cpp
    src/PerceptionSystem.cpp
    src/DangerSystem.cpp
    src/GoalPlanner.cpp
    src/HidePlanner.cpp
)

target_include_directories(OctoZone PRIVATE
    include
)
