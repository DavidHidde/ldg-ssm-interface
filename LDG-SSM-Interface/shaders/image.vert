#version 410

layout(location = 0) in vec3 vert_coord;
layout(location = 1) in vec2 tex_coord;

uniform mat4 model_view_projection_matrix;

layout(location = 0) out vec2 vertex_tex_coord;

void main(void)
{
    gl_Position = model_view_projection_matrix * vec4(vert_coord, 1.);
    vertex_tex_coord = tex_coord;
}
