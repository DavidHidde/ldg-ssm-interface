#version 410

const float voxel_width = 1.0 / 64.0;

flat in vec3 viewport;

uniform sampler3D volume;

uniform vec3 background_color;
uniform float num_samples;

layout(location = 0) out vec4 frag_color;

// Ray
struct Ray {
    vec3 origin;
    vec3 direction;
};

// Axis-aligned bounding box
struct BoundingBox {
    vec3 min;
    vec3 max;
};

// Forward declarations.
void findPosition(vec3 viewport, out BoundingBox bounding_box, out Ray ray);
bool intersectBoundingBox(Ray ray, BoundingBox bounding_box, out float t_near, out float t_far);
vec4 transferFunction(float value);

/**
 * Correct opacity for the current sampling rate
 *
 * @param alpha: input opacity.
 * @param samplingRatio: the ratio between current sampling rate and the original.
 */
float opacityCorrection(in float alpha, in float sample_ratio)
{
    float a_corrected = 1.0 - pow(1.0 - alpha, sample_ratio);
    return a_corrected;
}

/**
 * Accumulation composition
 *
 * @param sample: current sample value.
 * @param samplingRatio: the ratio between current sampling rate and the original. (ray step)
 * @param composedColor: blended color (both input and output)
 */
void accumulation(float value, float sample_ratio, inout vec4 composed_color)
{
    vec4 color = transferFunction(value);
    color.a = opacityCorrection(color.a, sample_ratio);

    composed_color.xyz += (1.0 - composed_color.a) * color.xyz;
    composed_color.a += (1.0 - composed_color.a) * color.a;
}

// Main raycasting loop
void main(void)
{
    Ray ray;
    BoundingBox bounding_box;
    findPosition(viewport, bounding_box, ray);

    float t_near, t_far;
    bool hit = intersectBoundingBox(ray, bounding_box, t_near, t_far);
    if (!hit) {
        frag_color = vec4(background_color, 1.);
        return;
    }

    float t_step = (bounding_box.max.x - bounding_box.min.x) / num_samples;
    vec4 final_color = vec4(0.0);
    // ratio between current sampling rate vs. the original sampling rate
    float sample_ratio = 1.0 / (num_samples * voxel_width);

    // Main raycasting loop
    float t = t_near;
    float maximum_intensity = 0.0;
    while(t < t_far) {
        // Normalize texture coordinates based on volume
        vec3 pos = (ray.origin + t * ray.direction - bounding_box.min) / (bounding_box.max - bounding_box.min);
        float value = texture(volume, pos).r;

        accumulation(value, sample_ratio, final_color);

        // Early stoppping
        if (final_color.a >= 1.)
            break;

        t += t_step;
    }
    final_color = transferFunction(maximum_intensity);
    frag_color = final_color * final_color.a + (1.0 - final_color.a) * vec4(background_color, 1.);
}
