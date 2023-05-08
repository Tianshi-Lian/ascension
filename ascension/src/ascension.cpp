/**
 * File: ascension.cpp
 * Project: ascension
 * File Created: 2023-04-13 20:17:48
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-05-08 19:29:20
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

#include "graphics/buffer_object.hpp"
#include "graphics/sprite_batch.hpp"
#include "graphics/texture_2d.hpp"

#include "graphics/vertex_array_object.hpp"
#include "yuki/debug/instrumentor.hpp"

namespace ascension {

const i32 WINDOW_WIDTH = 1600, WINDOW_HEIGHT = 900, OBJECT_COUNT = 100000;

GLuint shaderProgramId, textureId;

graphics::Vertex_Array_Object vao;
graphics::Vertex_Buffer_Object vbo;
graphics::Vertex_Buffer_Object ubo;
graphics::Index_Buffer_Object ibo;

const char* vertexShader = "#version 330\n"
                           "layout (location = 0) in vec2 vert;\n"
                           "layout (location = 1) in vec2 _uv;\n"
                           "out vec2 uv;\n"
                           "void main()\n"
                           "{\n"
                           "    uv = _uv;\n"
                           "    gl_Position = vec4(vert.x / 720.0 - 1.0, vert.y / 405.0 - 1.0, 0.0, 1.0);\n"
                           "}\n";

const char* fragmentShader = "#version 330\n"
                             "out vec4 color;\n"
                             "in vec2 uv;\n"
                             "uniform sampler2D tex;\n"
                             "void main()\n"
                             "{\n"
                             "    color = texture(tex, uv);\n"
                             "}\n";

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

GLuint
compileShader(const GLchar* source, GLuint shaderType)
{
    GLuint shaderHandler;

    shaderHandler = glCreateShader(shaderType);
    glShaderSource(shaderHandler, 1, &source, 0);
    glCompileShader(shaderHandler);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shaderHandler, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderHandler, 512, 0, infoLog);
        printf("Error in compilation of shader:\n%s\n", infoLog);
        exit(1);
    };

    return shaderHandler;
}

void
Ascension::on_initialize()
{
    m_asset_manager.load_asset_file("assets/assets.xml");
    m_asset_manager.load_texture_2d("textures/unicorn");
    auto fruit_tex = m_asset_manager.load_texture_2d("textures/fruits");
    textureId = fruit_tex->id();

    // viewport setup
    {
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    // initialize shader
    {
        GLuint programId, vertexHandler, fragmentHandler;

        vertexHandler = compileShader(vertexShader, GL_VERTEX_SHADER);
        fragmentHandler = compileShader(fragmentShader, GL_FRAGMENT_SHADER);

        programId = glCreateProgram();
        glAttachShader(programId, vertexHandler);
        glAttachShader(programId, fragmentHandler);
        glLinkProgram(programId);

        GLint success;
        GLchar infoLog[512];
        glGetProgramiv(programId, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(programId, 512, 0, infoLog);
            printf("Error in linking of shaders:\n%s\n", infoLog);
            exit(1);
        }

        glDeleteShader(vertexHandler);
        glDeleteShader(fragmentHandler);

        shaderProgramId = programId;
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

    vbo.create(sizeof(vertices));
    vao.set_attrib_ptr(2, 2 * sizeof(float), 0, false);

    ubo.create(sizeof(uvs), uvs);
    vao.set_attrib_ptr(2, 2 * sizeof(float), 0, true);

    ibo.create(sizeof(indices), indices);

    glBindVertexArray(0);

    // const auto sprite_shader = m_asset_manager.load_shader("shaders/spritebatch");
}

void
Ascension::on_update(f64 delta_time)
{
    (void)delta_time;

    if (m_input_manager.is_key_down(input::Key::ESCAPE)) {
        quit();
    }

    // dancing fruits =)
    for (i32 i = 0; i < OBJECT_COUNT; i++) {
        objects[i].x += static_cast<i16>((rand() % 5 - 2));
        objects[i].y += static_cast<i16>((rand() % 5 - 2));

        updateObject(i);
    }

    // if you had to unbind vbo for whatever reason, bind it again now
    vbo.bind();
    vbo.buffer_data(sizeof(vertices), vertices);
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
