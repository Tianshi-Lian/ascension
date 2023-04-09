/**
 * File: opengl_platform.hpp
 * Project: erika
 * File Created: 2023-03-27 19:45:50
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-09 16:04:14
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

#include "opengl_platform_types.hpp"

namespace yuki::platform {
struct Platform_State;
}

namespace erika::plugins::renderer::opengl {
class OpenGL_Platform {
  public:
    static bool create_context(
        const std::shared_ptr<OpenGL_Platform_State>& opengl_platform_state,
        const std::shared_ptr<yuki::platform::Platform_State>& platform_state
    );

    static void swap_buffers(const std::shared_ptr<OpenGL_Platform_State>& opengl_platform_state);
};

}
