#include "ascension.hpp"

#include <unordered_map>

#include "yuki/debug/logger.hpp"

#include "erika/plugins/plugin_manager.hpp"
#include "erika/plugins/renderer/d3d11/d3d11.hpp"

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

Ascension::Ascension()
  : erika::Game("Ascension")
{
}

bool
Ascension::on_initialize()
{
    using namespace erika::plugins;
    yuki::debug::Logger::info("ascension > Initializing game...");

    player.name = "Tianshi";
    player.plane = plane::MORTAL;
    player.rank = rank::NOVICE;
    player.cultivation = 0;

    player.aspect_mastery[aspect::FIRE] = 1.0f;
    player.path_mastery[path::ALCHEMY] = 0.1f;
    player.skills.push_back({ "Fire palm", aspect::FIRE, plane::MORTAL, skill::stage::novice, 0 });

    yuki::debug::Logger::notice("ascension > Game initialized.");

    {
        // TODO: Remove, temporary test.
        Plugin_Manager plugin_manager;

        const auto d3d11_plugin_factory = std::make_shared<renderer::d3d11::D3D11_Renderer_Factory>();
        plugin_manager.register_renderer("D3D11_Renderer", d3d11_plugin_factory);

        const auto renderer_plugins_available = plugin_manager.get_registered_renderers();
        yuki::debug::Logger::debug("Renderer plugin: %s", renderer_plugins_available.at(0).c_str());

        plugin_manager.change_active_renderer(renderer_plugins_available.at(0));

        auto renderer = plugin_manager.get_active_renderer();
        renderer->begin_scene();
        renderer->end_scene();
    }

    return true;
}

void
Ascension::on_update(f32 delta_time)
{
    (void)delta_time;
}

void
Ascension::on_render(f32 delta_time)
{
    (void)delta_time;
}

} // namespace ascension
