#version 410

layout(location = 0) in vec3 vertex_tex_coord;
uniform sampler2DArray node_textures;

layout(location = 0) out vec4 frag_color;

void main(void)
{
    frag_color = texture(node_textures, vertex_tex_coord);
}
