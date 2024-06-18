#version 410

layout(location = 0) in vec3 vert_coord;
layout(location = 1) in vec3 tex_coord;
layout(location = 2) in vec3 tex_coord_origin;          // Instanced
layout(location = 3) in mat4 instance_transformation;   // Instanced

uniform vec2 screen_origin;
uniform vec3 screen_space_projection;
uniform mat4 projection_matrix;

layout(location = 0) out vec3 vertex_tex_coord;

// Project a vector from screen space to world space
vec4 project(vec3 vector)
{
    return vec4(screen_space_projection, 1.) * (instance_transformation * vec4(vector, 1.) + vec4(screen_origin, 0., 0.)) - vec4(1., 1., 0., 0.);
}

void main(void)
{
    gl_Position = projection_matrix * project(vert_coord);
    vertex_tex_coord = tex_coord_origin + tex_coord;
}
