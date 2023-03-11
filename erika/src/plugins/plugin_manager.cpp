#include "plugins/plugin_manager.hpp"

#include <functional>

#include "yuki/debug/logger.hpp"

#include "erika/plugins/renderer.hpp"

namespace erika::plugins {

Plugin_Manager::Plugin_Manager()
  : m_active_renderer(nullptr)
{
}

void
Plugin_Manager::register_renderer(std::string name, const std::shared_ptr<Renderer_Plugin_Factory>& factory)
{
    const auto factory_exists =
        std::find_if(m_registered_renderers.begin(), m_registered_renderers.end(), [&name](auto& registered_factory) {
            return registered_factory.first == name;
        });

    if (factory_exists != m_registered_renderers.end()) {
        yuki::debug::Logger::error(
            "erika > Plugin_Manager attempting to register renderer factory %s more than once.", name.c_str()
        );
        return;
    }

    m_registered_renderers.emplace_back(std::move(name), factory);
}

void
Plugin_Manager::change_active_renderer(const std::string& name)
{
    const auto factory_exists =
        std::find_if(m_registered_renderers.begin(), m_registered_renderers.end(), [&name](auto& registered_factory) {
            return registered_factory.first == name;
        });

    if (factory_exists == m_registered_renderers.end()) {
        yuki::debug::Logger::error("erika > Plugin_Manager attempting to create unrecognised renderer %s.", name.c_str());
        return;
    }

    auto new_plugin = factory_exists->second->create();
    if (!new_plugin) {
        yuki::debug::Logger::error("erika > Plugin_Manager failed to create renderer plugin %s.", name.c_str());
        return;
    }

    if (m_active_renderer != nullptr) {
        m_active_renderer->shutdown();
        m_active_renderer.reset();
    }

    new_plugin->initialize();
    m_active_renderer = new_plugin;
}

std::vector<std::string>
Plugin_Manager::get_registered_renderers() const
{
    std::vector<std::string> results(m_registered_renderers.size());
    std::transform(m_registered_renderers.begin(), m_registered_renderers.end(), results.begin(), [](auto& renderer) {
        return renderer.first;
    });

    return results;
}

std::shared_ptr<Renderer>
Plugin_Manager::get_active_renderer() const
{
    return m_active_renderer;
}

}
