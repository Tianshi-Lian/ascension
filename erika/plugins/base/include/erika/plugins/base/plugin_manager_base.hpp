/**
 * File: plugin_manager_base.hpp
 * Project: erika
 * File Created: 2023-05-29 18:59:54
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-05-29 19:03:54
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

#ifndef ERIKA_PLUGINS_BASE_PLUGIN_MANAGER_BASE_HPP
#define ERIKA_PLUGINS_BASE_PLUGIN_MANAGER_BASE_HPP

namespace erika::plugins {

class Renderer;
template<typename T>
class Plugin_Factory;

/**
 * @class Plugin_Manager_Base
 *
 * @brief An interface for the plugin manager to specify the relevant register operations across libraries.
 */
class Plugin_Manager_Base {
  public:
    Plugin_Manager_Base() = default;
    virtual ~Plugin_Manager_Base() = default;

    /**
     * @brief Register a renderer plugin with the manager.
     *
     * @param     name      the name of the renderer plugin
     * @param     factory   a shared pointer to a factory which can create this renderer plugin
     */
    virtual void register_renderer(std::string name, const std::shared_ptr<Plugin_Factory<Renderer>>& factory) = 0;

    Plugin_Manager_Base(const Plugin_Manager_Base&) = default;
    Plugin_Manager_Base(Plugin_Manager_Base&&) = delete;
    Plugin_Manager_Base& operator=(const Plugin_Manager_Base&) = default;
    Plugin_Manager_Base& operator=(Plugin_Manager_Base&&) = delete;
};

}

#endif // ERIKA_PLUGINS_BASE_PLUGIN_MANAGER_BASE_HPP
