#version 430 core
in vec2 f_tex_coords;
// in vec4 f_color;

out vec4 f_frag_color;

uniform sampler2D u_texture; // TODO: texture array(s)?

void main()
{
    f_frag_color = texture(u_texture, f_tex_coords);// * f_color;
}
