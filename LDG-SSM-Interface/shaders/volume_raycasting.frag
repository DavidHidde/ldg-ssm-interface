#version 410

const float num_samples = 150;
const float voxel_width = 1.0 / 64.0;

const vec3 colorNode0 = vec3(0, 0, 1);  // blue
const vec3 colorNode1 = vec3(1, 1, 1);  // white
const vec3 colorNode2 = vec3(1, 0, 0);  // red

layout(pixel_center_integer,  origin_upper_left) in vec4 gl_FragCoord;

flat in vec3 viewport;

out vec4 f_color;

uniform sampler3D volume;
uniform vec3 screen_space_projection;
uniform mat4 model_view_matrix;
uniform mat3 input_bounding_box;

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

/**
 * Evaluates the transfer function for a given sample value
 *
 * @param value The sample value
 * @return The color for the given sample value
 */
vec4 transferFunction(float value)
{
    float alpha = value * 0.1; // value;
    if (value < 0.2)
        alpha = 0.5;

    float t = 0.0;
    vec3 color_0 = colorNode0;
    vec3 color_1 = colorNode1;
    if (value < 0.5) {
        t = 2.0 * value;
    } else  {
        t = 2.0 * (value - 0.5);
        color_0 = colorNode1;
        color_1 = colorNode2;
    }
    vec4 color;
    color.a = alpha;
    color.rgb = color_0 * (1.0 - t) + color_1 * t;
    return color;
}

/**
 * Correct opacity for the current sampling rate
 *
 * @param alpha: input opacity.
 * @param samplingRatio: the ratio between current sampling rate and the original.
 */
float opacityCorrection(in float alpha, in float sampling_ratio)
{
    float a_corrected = 1.0 - pow(1.0 - alpha, sampling_ratio);
    return a_corrected;
}

/**
 * Accumulation composition
 *
 * @param sample: current sample value.
 * @param samplingRatio: the ratio between current sampling rate and the original. (ray step)
 * @param composedColor: blended color (both input and output)
 */
void accumulation(float value, float sampleRatio, inout vec4 composed_color)
{
    vec4 color = transferFunction(value);
    color.a = opacityCorrection(color.a, sampleRatio);

    // Old accumulation method
    composed_color += (1.0 - composed_color.a) * color * color.a;
    composed_color.a += (1.0 - composed_color.a) * color.a;
}

/**
 * Intersects a ray with the bounding box and sets the intersection points
 * Returns true if the ray intersects the bounding box, false otherwise.
 */
bool intersectBoundingBox(Ray ray, BoundingBox bounding_box, out float t_near, out float t_far)
{
    vec3 inverse_ray_dir = vec3(1.) / ray.direction;
    vec3 t_bot = inverse_ray_dir * (bounding_box.min - ray.origin);
    vec3 t_top = inverse_ray_dir * (bounding_box.max - ray.origin);

    vec3 t_min = min(t_top, t_bot);
    vec3 t_max = max(t_top, t_bot);

    float max_t_min = max(max(t_min.x, t_min.y), max(t_min.x, t_min.z));
    float min_t_max = min(min(t_max.x, t_max.y), min(t_max.x, t_max.z));

    t_near = max_t_min;
    t_far = min_t_max;

    return (t_far > t_near);
}

void main(void)
{
    // Figure out normalized location on screen
    vec3 relative_pos = vec3(2. * (gl_FragCoord.xy - viewport.xy) / vec2(viewport.z, viewport.z) - 1., input_bounding_box[2].z + 2);
    relative_pos = vec3(model_view_matrix * vec4(relative_pos, 1.));
    Ray ray;
    ray.origin = vec3(model_view_matrix * vec4(input_bounding_box[2], 1.));
    ray.direction = normalize(relative_pos - ray.origin);

    BoundingBox bounding_box;
    bounding_box.min = input_bounding_box[0];
    bounding_box.max = input_bounding_box[1];

    float t_near, t_far;
    vec4 background = vec4(0., 0., 0., 1.0);
    bool hit = intersectBoundingBox(ray, bounding_box, t_near, t_far);
    if (!hit) {
        f_color = background;
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
        if (value > maximum_intensity) {
            maximum_intensity = value;
        }

        accumulation(value, sample_ratio, final_color);

        t += t_step;
    }
    final_color = transferFunction(maximum_intensity);
    f_color = final_color * final_color.a + (1.0 - final_color.a) * background;
}
