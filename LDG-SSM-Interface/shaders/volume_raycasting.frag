#version 410

in vec3 frag_coords;
flat in vec3 projection_origin;

out vec4 fColor;

uniform samplerBuffer volume_data;

void main(void)
{
    float color = texelFetch(volume_data, 0).r;
    fColor = vec4(color, color, color, 1.);
}
