#version 410

in vec3 frag_coords;
flat in vec3 projection_origin;

out vec4 fColor;

uniform samplerBuffer volume_data;

void main(void)
{
    fColor = vec4(1., 1., 1., 1.);
}
