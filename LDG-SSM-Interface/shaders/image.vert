#version 410

layout(location = 0) in vec3 vert_coord;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in uint tex_index;

uniform mat4 projection_matrix;

layout(location = 0) out vec3 vertex_tex_coord;

void main(void)
{
    gl_Position = projection_matrix * vec4(vert_coord, 1.);
    vertex_tex_coord = vec3(tex_coord, tex_index);
}
