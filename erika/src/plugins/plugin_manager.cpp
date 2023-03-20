#include "plugins/plugin_manager.hpp"

#include <filesystem>
#include <functional>

#include "yuki/debug/logger.hpp"
#include "yuki/platform/platform.hpp"

#include "plugins/plugin_types.hpp"
#include "plugins/renderer.hpp"

namespace erika::plugins {

Plugin_Manager::~Plugin_Manager()
{
    for (const auto& plugin : m_loaded_plugins) {
        yuki::platform::Platform::free_shared_library(plugin);
    }
}

void
Plugin_Manager::initialize()
{
    const auto plugin_types = magic_enum::enum_entries<Plugin_Type>();
    for (const auto& plugin_type : plugin_types) {
        if (plugin_type.first == Plugin_Type::Unknown) {
            continue;
        }

        std::string plugin_type_dir{ plugin_type.second };
        std::transform(plugin_type_dir.begin(), plugin_type_dir.end(), plugin_type_dir.begin(), ::tolower);

        if (!std::filesystem::exists(plugin_type_dir)) {
            continue;
        }

        // TODO: handle loading linux shared libraries.
        // /t We will probably want to check the current platform instead of naively just trying to load a different extension
        // /t as we don't want to be loading dll files on linux or vice versa.
        for (const auto& file : std::filesystem::directory_iterator{ plugin_type_dir }) {
            if (!file.is_regular_file() || file.path().extension() != ".dll") {
                continue;
            }

            const auto plugin_name = file.path().filename().string();

            yuki::platform::Library_Handle plugin_lib;
            try {
                plugin_lib = yuki::platform::Platform::load_shared_library(file.path().string());

                if (plugin_lib.internal_state == nullptr) {
                    throw std::runtime_error("Failed to load library file");
                }

                const auto plugin_load = yuki::platform::Platform::load_library_function<void(erika::plugins::Plugin_Manager&)>(
                    plugin_lib, "registerPlugin"
                );

                if (plugin_load == nullptr) {
                    throw std::runtime_error("Failed to find registerPlugin() function");
                }

                plugin_load(*this);
                yuki::debug::Logger::debug("Loaded %s plugin %s", plugin_type.second.begin(), plugin_name.c_str());
            }
            catch (const std::exception& e) {
                yuki::platform::Platform::free_shared_library(plugin_lib);
                yuki::debug::Logger::error("Failed to load plugin %s with error %s", file.path().string().c_str(), e.what());
                continue;
            }
        }
    }
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
