#version 410

layout(location = 0) in vec3 vertex_tex_coord;
uniform sampler2DArray node_textures;

out vec4 fColor;

void main(void)
{
    fColor = texture(node_textures, vertex_tex_coord);
}
