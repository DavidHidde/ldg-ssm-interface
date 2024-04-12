#version 410

layout(location = 0) in vec2 vertex_tex_coord;
uniform sampler2D node_texture;

out vec4 fColor;

void main(void)
{
    fColor = texture(node_texture, vertex_tex_coord);
}
