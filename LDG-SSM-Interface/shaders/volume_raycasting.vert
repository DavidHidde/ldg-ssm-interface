#version 410

layout(location = 0) in vec3 vert_coord;
layout(location = 1) in mat4 instance_transformation;

uniform vec3 screen_space_projection;
uniform mat4 projection_matrix;
uniform mat3 input_bounding_box;

flat out mat3 bounding_box_coords;

// Project a vector from screen space to world space
vec4 project(vec3 vector)
{
    return vec4(screen_space_projection, 1.) * (instance_transformation * vec4(vector, 1.)) - vec4(1., 1., 0., 0.);
}

void main(void)
{
    bounding_box_coords = input_bounding_box;

    bounding_box_coords[0] = vec3(project(bounding_box_coords[0]));
    bounding_box_coords[1] = vec3(project(bounding_box_coords[1]));
    bounding_box_coords[2] = vec3(project(bounding_box_coords[2]));

    gl_Position = projection_matrix * project(vert_coord);
}
