#version 430 core
layout (location = 0) in vec2 v_position;
layout (location = 1) in vec2 v_tex_coords;
layout (location = 2) in vec4 v_color;

out vec2 f_tex_coords;
out vec4 f_color;

uniform mat4 m_projection_view;

void main()
{
    f_tex_coords = v_tex_coords;
    f_color = v_color;
    gl_Position = m_projection_view * vec4(v_position.x, v_position.y, 0.0, 1.0);
}
