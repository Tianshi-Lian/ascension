/**
 * File: ascension.cpp
 * Project: ascension
 * File Created: 2023-04-13 20:17:48
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-07-05 18:40:51
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

#include "ascension.hpp"

#include <GL/glew.h>
#include <glm/ext/matrix_clip_space.hpp>

#include "graphics/shader.hpp"
#include "graphics/texture_2d.hpp"

#include "yuki/debug/instrumentor.hpp"

namespace ascension {

const i32 WINDOW_WIDTH = 1600, WINDOW_HEIGHT = 900, OBJECT_COUNT = 100000;

struct Texture {
    v2i size;
    float u1, v1, u2, v2;
};
struct Object {
    v2f position;
    Texture texture;
};

Texture watermelon = { { 64, 64 }, 0.0f, 0.5f, 0.5f, 1.0f };
Texture pineapple = { { 64, 64 }, 0.5f, 0.5f, 1.0f, 1.0f };
Texture orange = { { 32, 32 }, 0.0f, 0.25f, 0.25f, 0.5f };
Texture grape = { { 32, 32 }, 0.25f, 0.25f, 0.5f, 0.5f };
Texture pear = { { 32, 32 }, 0.0f, 0.0f, 0.25f, 0.25f };
Texture banana = { { 32, 32 }, 0.25f, 0.0f, 0.5f, 0.25f };
Texture strawberry = { { 16, 16 }, 0.5f, 0.375f, 0.625f, 0.5f };
Texture raspberry = { { 16, 16 }, 0.625f, 0.375f, 0.75f, 0.5f };
Texture cherry = { { 16, 16 }, 0.5f, 0.25f, 0.625f, 0.375f };

Texture textures[9] = { watermelon, pineapple, orange, grape, pear, banana, strawberry, raspberry, cherry };

static Object objects[OBJECT_COUNT];

void
Ascension::on_initialize()
{
    m_asset_manager.load_asset_file("assets/assets.xml");
    m_asset_manager.load_texture_2d("textures/unicorn");
    auto fruit_tex = m_asset_manager.load_texture_2d("textures/fruits");
    auto sprite_shader = m_asset_manager.load_shader("shaders/spritebatch");

    // viewport setup
    {
        auto projection = glm::ortho(0.0f, 1600.0f, 0.0f, 900.0f, -1.0f, 1.0f);
        sprite_shader->bind();
        sprite_shader->set_mat4f("m_projection_view", projection * m4{ 1.0f });
    }

    m_batch.create(16, 2048, sprite_shader);

    auto fruits = std::make_shared<graphics::Batch>();
    fruits->create({ OBJECT_COUNT, fruit_tex, sprite_shader, true });

    for (auto& object : objects) {
        Texture t = textures[rand() % 9];
        object = { v2f{ static_cast<i16>((rand() % (WINDOW_WIDTH - t.size.x))),
                        static_cast<i16>((rand() % (WINDOW_HEIGHT - t.size.y))) },
                   t };

        fruits->add(
            object.position,
            object.texture.size,
            v4f{ object.texture.u1, object.texture.v1, object.texture.u2, object.texture.v2 }
        );
    }

    m_batch.add_batch(fruits);
}

void
Ascension::on_update(f64 delta_time)
{
    (void)delta_time;

    if (m_input_manager.is_key_down(input::Key::ESCAPE)) {
        quit();
    }
}

void
Ascension::on_render(f32 interpolation)
{
    PROFILE_FUNCTION();
    (void)interpolation;

    m_batch.flush();
}
}
