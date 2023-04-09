/**
 * File: platform_types.hpp
 * Project: yuki
 * File Created: 2023-03-20 18:40:53
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-09 16:05:09
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

#include <memory>

namespace yuki {

/**
 * @struct Platform_State
 *
 * @brief A struct holding platform-independent state.
 */
struct Platform_State {
    std::shared_ptr<void> internal_state;
};

/**
 * @struct Library_Handle
 *
 * @brief A struct holding a platform-independent handle to a shared library.
 */
struct Library_Handle {
    std::string filepath;
    std::shared_ptr<void> internal_state;
};
}
