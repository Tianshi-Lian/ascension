#include "ascension.hpp"

#include <unordered_map>

namespace ascension {

struct skill {
    enum class stage {
        novice,
        adept,
        master
    };

    std::string name;
    ascension::aspect aspect;
    ascension::plane rank;

    skill::stage stage;
    u64 experience;
};

struct player {
    std::string name;

    ascension::plane plane;
    ascension::rank rank;
    u64 cultivation;

    std::unordered_map<aspect, f32> aspect_mastery;
    std::unordered_map<path, f32> path_mastery;
    std::vector<skill> skills;
};

player player;

void
ascension::Ascension::initialize()
{
    player.name = "Tianshi";
    player.plane = plane::MORTAL;
    player.rank = rank::NOVICE;
    player.cultivation = 0;

    player.aspect_mastery[aspect::FIRE] = 1.0f;
    player.path_mastery[path::ALCHEMY] = 0.1f;
    player.skills.push_back({ "Fire palm", aspect::FIRE, plane::MORTAL, skill::stage::novice, 0 });
}

void
ascension::Ascension::run()
{
}

} // namespace ascension
