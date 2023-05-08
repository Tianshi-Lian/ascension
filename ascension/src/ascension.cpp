/**
 * File: ascension.cpp
 * Project: ascension
 * File Created: 2023-04-13 20:17:48
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-05-08 21:33:22
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

#include "graphics/buffer_object.hpp"
#include "graphics/shader.hpp"
#include "graphics/sprite_batch.hpp"
#include "graphics/texture_2d.hpp"

#include "graphics/vertex_array_object.hpp"
#include "yuki/debug/instrumentor.hpp"

namespace ascension {

const i32 WINDOW_WIDTH = 1600, WINDOW_HEIGHT = 900, OBJECT_COUNT = 1500000;

GLuint shaderProgramId, textureId;

graphics::Vertex_Array_Object vao;
std::shared_ptr<graphics::Vertex_Buffer_Object> vbo;
std::shared_ptr<graphics::Vertex_Buffer_Object> ubo;
std::shared_ptr<graphics::Index_Buffer_Object> ibo;

struct Texture {
    i16 width, height;
    float u1, v1, u2, v2;
};
struct Object {
    i16 x, y;
    Texture texture;
};

Texture watermelon = { 64, 64, 0.0f, 0.5f, 0.5f, 1.0f };
Texture pineapple = { 64, 64, 0.5f, 0.5f, 1.0f, 1.0f };
Texture orange = { 32, 32, 0.0f, 0.25f, 0.25f, 0.5f };
Texture grape = { 32, 32, 0.25f, 0.25f, 0.5f, 0.5f };
Texture pear = { 32, 32, 0.0f, 0.0f, 0.25f, 0.25f };
Texture banana = { 32, 32, 0.25f, 0.0f, 0.5f, 0.25f };
Texture strawberry = { 16, 16, 0.5f, 0.375f, 0.625f, 0.5f };
Texture raspberry = { 16, 16, 0.625f, 0.375f, 0.75f, 0.5f };
Texture cherry = { 16, 16, 0.5f, 0.25f, 0.625f, 0.375f };

Texture textures[9] = { watermelon, pineapple, orange, grape, pear, banana, strawberry, raspberry, cherry };

static Object objects[OBJECT_COUNT];

static float vertices[OBJECT_COUNT * 8];
static u16 indices[OBJECT_COUNT * 6];
static float uvs[OBJECT_COUNT * 8];

void
updateObject(int i)
{
    vertices[i * 8 + 0] = objects[i].x;
    vertices[i * 8 + 1] = objects[i].y;

    vertices[i * 8 + 2] = objects[i].x;
    vertices[i * 8 + 3] = objects[i].y + objects[i].texture.height;

    vertices[i * 8 + 4] = objects[i].x + objects[i].texture.width;
    vertices[i * 8 + 5] = objects[i].y + objects[i].texture.height;

    vertices[i * 8 + 6] = objects[i].x + objects[i].texture.width;
    vertices[i * 8 + 7] = objects[i].y;
}

void
Ascension::on_initialize()
{
    m_asset_manager.load_asset_file("assets/assets.xml");
    m_asset_manager.load_texture_2d("textures/unicorn");
    auto fruit_tex = m_asset_manager.load_texture_2d("textures/fruits");
    textureId = fruit_tex->id();
    auto sprite_shader = m_asset_manager.load_shader("shaders/spritebatch");
    shaderProgramId = sprite_shader->id();

    // viewport setup
    {
        auto projection = glm::ortho(0.0f, 1600.0f, 0.0f, 900.0f, -1.0f, 1.0f);
        sprite_shader->bind();
        sprite_shader->set_mat4f("m_projection_view", projection * m4{ 1.0f });
    }

    for (u32 i = 0; i < OBJECT_COUNT; i++) {
        Texture t = textures[rand() % 9];
        objects[i] = { static_cast<i16>((rand() % (WINDOW_WIDTH - t.width))),
                       static_cast<i16>((rand() % (WINDOW_HEIGHT - t.height))),
                       t };

        // vertices
        {
            vertices[i * 8 + 0] = objects[i].x;
            vertices[i * 8 + 1] = objects[i].y;

            vertices[i * 8 + 2] = objects[i].x;
            vertices[i * 8 + 3] = objects[i].y + objects[i].texture.height;

            vertices[i * 8 + 4] = objects[i].x + objects[i].texture.width;
            vertices[i * 8 + 5] = objects[i].y + objects[i].texture.height;

            vertices[i * 8 + 6] = objects[i].x + objects[i].texture.width;
            vertices[i * 8 + 7] = objects[i].y;
        }

        // uvs
        {
            uvs[i * 8 + 0] = objects[i].texture.u1;
            uvs[i * 8 + 1] = objects[i].texture.v1;

            uvs[i * 8 + 2] = objects[i].texture.u1;
            uvs[i * 8 + 3] = objects[i].texture.v2;

            uvs[i * 8 + 4] = objects[i].texture.u2;
            uvs[i * 8 + 5] = objects[i].texture.v2;

            uvs[i * 8 + 6] = objects[i].texture.u2;
            uvs[i * 8 + 7] = objects[i].texture.v1;
        }

        // indices
        {
            static const std::array<u16, 6> indices_template{ 0, 1, 2, 2, 3, 0 };
            const u32 vertex_offset = i * 4;

            indices[i * 6 + 0] = static_cast<u16>((indices_template.at(0) + vertex_offset));
            indices[i * 6 + 1] = static_cast<u16>((indices_template.at(1) + vertex_offset));
            indices[i * 6 + 2] = static_cast<u16>((indices_template.at(2) + vertex_offset));

            indices[i * 6 + 3] = static_cast<u16>((indices_template.at(3) + vertex_offset));
            indices[i * 6 + 4] = static_cast<u16>((indices_template.at(4) + vertex_offset));
            indices[i * 6 + 5] = static_cast<u16>((indices_template.at(5) + vertex_offset)); // NOLINT
        }
    }

    // initialize OpenGL buffers
    vao.create(true);

    vbo = std::make_shared<graphics::Vertex_Buffer_Object>();
    vbo->create(sizeof(vertices), vertices);
    vbo->set_layout({ { graphics::Shader_Data_Type::Float, 2, false } });
    vao.add_vertex_buffer(vbo);

    ubo = std::make_shared<graphics::Vertex_Buffer_Object>();
    ubo->create(sizeof(uvs), uvs);
    ubo->set_layout({ { graphics::Shader_Data_Type::Float, 2, true } });
    vao.add_vertex_buffer(ubo);

    ibo = std::make_shared<graphics::Index_Buffer_Object>();
    ibo->create(sizeof(indices), indices);
    vao.set_index_buffer(ibo);

    vao.unbind();
}

void
Ascension::on_update(f64 delta_time)
{
    (void)delta_time;

    if (m_input_manager.is_key_down(input::Key::ESCAPE)) {
        quit();
    }

    // dancing fruits =)
    // for (i32 i = 0; i < OBJECT_COUNT; i++) {
    //     objects[i].x += static_cast<i16>((rand() % 5 - 2));
    //     objects[i].y += static_cast<i16>((rand() % 5 - 2));

    //     updateObject(i);
    // }

    // if you had to unbind vbo for whatever reason, bind it again now
    // vbo->buffer_data(sizeof(vertices), vertices);
}

void
Ascension::on_render(f32 interpolation)
{
    PROFILE_FUNCTION();
    (void)interpolation;

    // if you had to unbind vao for whatever reason, bind it again now
    vao.bind();
    glUseProgram(shaderProgramId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // glDrawArrays(GL_TRIANGLES, 0, OBJECT_COUNT * 6);
    // ibo.bind();
    glDrawElements(GL_TRIANGLES, OBJECT_COUNT * 6, GL_UNSIGNED_SHORT, nullptr);

    // m_sprite_batch.begin();
    // for (int i = 0; i < 1000; ++i) {
    //     m_sprite_batch.draw(m_asset_manager.get_texture_2d("textures/unicorn"), v2f{ 0.0f, 0.0f });
    // }
    // m_sprite_batch.end();
}

}
