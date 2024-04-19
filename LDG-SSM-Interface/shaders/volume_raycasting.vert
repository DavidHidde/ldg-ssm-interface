#version 410

layout(location = 0) in vec3 vert_coord;
layout(location = 1) in uint projection_idx;

uniform mat4 model_view_projection_matrix;

flat out uint vertex_projection_idx;

void main(void)
{
    gl_Position = model_view_projection_matrix * vec4(vert_coord, 1.);
    vertex_projection_idx = projection_idx;
}
