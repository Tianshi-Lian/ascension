/**
 * File: engine.hpp
 * Project: erika
 * File Created: 2023-03-03 18:32:58
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-09 16:03:44
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

namespace yuki {
struct Platform_State;
}

namespace erika {

class Game;

/**
 * @class Engine
 *
 * @brief Core engine class for the erika game engine.
 */
class Engine final {
  public:
    /**
     * @brief Initialize the game engine.
     */
    static void initialize();
    /**
     * @brief Start running a game on top of the game engine.
     *
     * @param     platform_state      a shared pointer to the current platform state
     * @param     game      a shared pointer to the game to execute
     */
    static void run(const std::shared_ptr<yuki::Platform_State>& platform_state, const std::shared_ptr<Game>& game);
};

}
