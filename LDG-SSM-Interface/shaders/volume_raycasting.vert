#version 410

layout(location = 0) in vec3 vert_coord;
layout(location = 1) in mat4 instance_transformation;
// layout(location = 2) in uint[] volume_data;
// layout(location = 3) in vec3 projection_origin;

uniform vec3 screen_space_projection;
uniform mat4 model_view_projection_matrix;

flat out vec3 vertex_projection_origin;

void main(void)
{
    gl_Position = model_view_projection_matrix * (vec4(screen_space_projection, 1.) * (instance_transformation * vec4(vert_coord, 1.)) - vec4(1., 1., 0., 0.));
    vertex_projection_origin = vec3(0.);
}
