/**
 * File: types.hpp
 * Project: erika
 * File Created: 2023-05-29 19:08:14
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-05-29 19:08:38
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

#ifndef ERIKA_PLUGINS_RENDERER_TYPES_HPP
#define ERIKA_PLUGINS_RENDERER_TYPES_HPP

// TODO: Move this to an erika_common?

#include <glm/glm.hpp>

#include "yuki/types.hpp" // IWYU pragma: export

using v1 = glm::f32vec1;
using v2 = glm::f32vec2;
using v3 = glm::f32vec3;
using v4 = glm::f32vec4;

using v1f = v1;
using v2f = v2;
using v3f = v3;
using v4f = v4;

using v1i = glm::ivec1;
using v2i = glm::ivec2;
using v3i = glm::ivec3;
using v4i = glm::ivec4;

using v1u = glm::uvec1;
using v2u = glm::uvec2;
using v3u = glm::uvec3;
using v4u = glm::uvec4;

using m2 = glm::f32mat2;
using m3 = glm::f32mat3;
using m4 = glm::f32mat4;

using m2f = glm::f32mat2;
using m3f = glm::f32mat3;
using m4f = glm::f32mat4;

#endif // ERIKA_PLUGINS_RENDERER_TYPES_HPP
