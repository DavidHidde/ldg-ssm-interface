#version 410

layout(location = 0) in vec3 vert_coord;
layout(location = 1) in vec3 input_viewport;
layout(location = 2) in mat4 instance_transformation;

uniform vec3 screen_space_projection;
uniform mat4 projection_matrix;

flat out vec3 viewport;

// Project a vector from screen space to world space
vec4 project(vec3 vector)
{
    return vec4(screen_space_projection, 1.) * (instance_transformation * vec4(vector, 1.)) - vec4(1., 1., 0., 0.);
}

void main(void)
{
    viewport = input_viewport;
    gl_Position = projection_matrix * project(vert_coord);
}
