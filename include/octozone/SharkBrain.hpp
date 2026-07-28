#pragma once

#include "octozone/Grid.hpp"
#include "octozone/Path.hpp"
#include "octozone/Position.hpp"
#include "octozone/Shark.hpp"

namespace octozone
{
    class SharkBrain
    {
    public:
        void update(
            Shark& shark,
            const Grid& grid,
            Position octopusPosition,
            bool octopusHidden,
            const Path& occupiedPositions) const;

    private:
        static constexpr int searchTurns_{3};

        void updatePatrol(
            Shark& shark,
            const Grid& grid,
            const Path& occupiedPositions) const;

        void updateChase(
            Shark& shark,
            const Grid& grid,
            Position octopusPosition,
            bool octopusHidden,
            const Path& occupiedPositions) const;

        void updateSearch(
            Shark& shark,
            const Grid& grid,
            Position octopusPosition,
            bool octopusHidden,
            const Path& occupiedPositions) const;

        void updateReturnToPatrol(
            Shark& shark,
            const Grid& grid,
            const Path& occupiedPositions) const;

        bool moveToward(
            Shark& shark,
            const Grid& grid,
            Position target,
            const Path& occupiedPositions) const;
    };
}
