#version 410

layout(location = 0) in vec3 vert_coord;
layout(location = 1) in mat4 instance_transformation;
layout(location = 5) in vec3 input_projection_origin;

uniform vec3 screen_space_projection;
uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;

out vec3 frag_coords;
flat out vec3 projection_origin;

// Project a vector from screen space to world space
vec4 project(vec3 vector)
{
    return vec4(screen_space_projection, 1.) * (instance_transformation * vec4(vector, 1.)) - vec4(1., 1., 0., 0.);
}

void main(void)
{
    frag_coords = vec3(model_view_matrix * project(vert_coord));
    projection_origin = vec3(project(input_projection_origin));

    gl_Position = projection_matrix * model_view_matrix * project(vert_coord);
}
