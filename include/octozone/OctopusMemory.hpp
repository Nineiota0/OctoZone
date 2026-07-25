#pragma once

#include "octozone/Grid.hpp"
#include "octozone/Octopus.hpp"
#include "octozone/Shark.hpp"

#include <optional>
#include <vector>

namespace octozone
{
    class OctopusMemory
    {
    public:
        void update(
            const Grid& grid,
            const Octopus& octopus,
            const std::vector<Shark>& sharks);

        std::vector<Shark> getKnownSharks() const;

    private:
        struct RememberedShark
        {
            Shark shark;
            int turnsSinceSeen{};
        };

        static constexpr int maxTurnsRemembered_{8};

        std::vector<std::optional<RememberedShark>> rememberedSharks_;
    };
}
