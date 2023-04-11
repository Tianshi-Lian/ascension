/**
 * File: plugin_manager.cpp
 * Project: erika
 * File Created: 2023-03-11 20:05:16
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-11 20:18:16
 * ------------------
 * Copyright 2023 Rob Graham
 * ==================
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * ==================
 */

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
        yuki::Platform::free_shared_library(plugin);
    }
}

void
Plugin_Manager::initialize(const std::shared_ptr<yuki::Platform_State>& platform_state)
{
    m_platform_state = platform_state;

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

            yuki::Library_Handle plugin_lib;
            try {
                plugin_lib = yuki::Platform::load_shared_library(file.path().string());

                if (plugin_lib.internal_state == nullptr) {
                    throw std::runtime_error("Failed to load library file");
                }

                const auto plugin_load =
                    yuki::Platform::load_library_function<void(erika::plugins::Plugin_Manager&)>(plugin_lib, "registerPlugin");

                if (plugin_load == nullptr) {
                    throw std::runtime_error("Failed to find registerPlugin() function");
                }

                plugin_load(*this);
                yuki::debug::Logger::debug("erika", "Loaded {} plugin {}", plugin_type.second.begin(), plugin_name);
            }
            catch (const std::exception& e) {
                yuki::Platform::free_shared_library(plugin_lib);
                yuki::debug::Logger::error("erika", "Failed to load plugin {}} with error {}", file.path(), e.what());
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
        yuki::debug::Logger::error("erika", "Plugin_Manager attempting to register renderer factory {} more than once.", name);
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
        yuki::debug::Logger::error("erika", "Plugin_Manager attempting to create unrecognised renderer {}.", name);
        return;
    }

    auto new_plugin = factory_exists->second->create();
    if (!new_plugin) {
        yuki::debug::Logger::error("erika", "Plugin_Manager failed to create renderer plugin {}.", name);
        return;
    }

    if (m_active_renderer != nullptr) {
        m_active_renderer->shutdown();
        m_active_renderer.reset();
    }

    new_plugin->initialize(m_platform_state);
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
