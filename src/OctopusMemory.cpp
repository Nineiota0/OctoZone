#include "octozone/OctopusMemory.hpp"
#include "octozone/PerceptionSystem.hpp"

namespace octozone
{
    void OctopusMemory::update(
        const Grid& grid,
        const Octopus& octopus,
        const std::vector<Shark>& sharks)
    {
        if (rememberedSharks_.size() != sharks.size())
        {
            rememberedSharks_.clear();
            rememberedSharks_.resize(sharks.size());
        }

        for (std::size_t index = 0; index < sharks.size(); ++index)
        {
            const Shark& shark = sharks[index];

            if (PerceptionSystem::canSeeShark(
                    grid,
                    octopus.getPosition(),
                    shark.getPosition()))
            {
                rememberedSharks_[index] = RememberedShark{shark, 0};
                continue;
            }

            if (!rememberedSharks_[index].has_value())
            {
                continue;
            }

            RememberedShark& memory = rememberedSharks_[index].value();
            ++memory.turnsSinceSeen;

            if (memory.turnsSinceSeen > maxTurnsRemembered_)
            {
                rememberedSharks_[index].reset();
                continue;
            }

            Shark::Projection projection =
                memory.shark.projectAfterOctopusMove(
                    grid,
                    octopus.getPosition());

            memory.shark.moveTo(projection.position);
            memory.shark.setState(projection.state);
        }
    }

    std::vector<Shark> OctopusMemory::getKnownSharks() const
    {
        std::vector<Shark> knownSharks;

        for (const std::optional<RememberedShark>& memory : rememberedSharks_)
        {
            if (memory.has_value())
            {
                knownSharks.push_back(memory->shark);
            }
        }

        return knownSharks;
    }
}
