/**
 * File: game.hpp
 * Project: erika
 * File Created: 2023-03-03 18:13:29
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-09 16:03:53
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

#pragma once

#include "erika/plugins/plugin_manager.hpp"

namespace yuki {
struct Platform_State;
}

namespace erika {

/**
 * @class Game
 *
 * @brief A base class for all games which run on top of the erika engine.
 */
class Game {
    friend class Engine;

public:
    explicit Game(std::string window_title = "Erika Game");
    virtual ~Game() = default;

    [[nodiscard]] const std::string& get_window_title() const;

    Game(const Game&) = default;
    Game(Game&&) = delete;
    Game& operator=(const Game&) = default;
    Game& operator=(Game&&) = delete;

protected:
    /**
     * @brief Callback for when the game should run initialization code.
     */
    virtual bool on_initialize() = 0;
    /**
     * @brief Callback for when the game should run its update logic.
     *
     * @param     delta_time    the time elapsed in seconds since the last update
     */
    virtual void on_update(f32 delta_time) = 0;
    /**
     * @brief Callback from when the game should run its render logic.
     *
     * @param     interpolation    interpolation value between 0 and 1 relative to current update frames progress
     */
    virtual void on_render(f32 interpolation) = 0;

    /**
     * @brief Set the window title for the games application.
     *
     * @param     value     string holding the value to set the title to
     */
    void set_window_title(std::string value);

    /**
     * @brief The created platform state which can be used to access platform-specific methods.
     */
    std::shared_ptr<yuki::Platform_State> m_platform_state;

    erika::plugins::Plugin_Manager m_plugin_manager;

private:
    bool initialize();
    void update(f32 delta_time);
    void render(f32 interpolation);

    void set_platform_state(const std::shared_ptr<yuki::Platform_State>& platform_state);

    std::string m_window_title;
};

namespace game {

std::shared_ptr<Game>
create();

}

}
