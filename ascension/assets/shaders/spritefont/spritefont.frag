#version 430 core
in vec2 f_tex_coords;
// in vec4 f_color;

out vec4 f_frag_color;

uniform sampler2D u_texture; // TODO: texture array(s)?

void main()
{
    vec4 sampled_texture = vec4(1.0, 1.0, 1.0, texture(u_texture, f_tex_coords).r);
    f_frag_color = sampled_texture; // * f_color
}
