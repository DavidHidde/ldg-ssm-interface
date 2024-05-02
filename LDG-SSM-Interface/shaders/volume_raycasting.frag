#version 410

layout(pixel_center_integer) in vec4 gl_FragCoord;

uniform vec3 color_0;
uniform vec3 color_1;
uniform vec3 color_2;

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
    vec3 blend_color_0 = color_0;
    vec3 blend_color_1 = color_1;
    if (value < 0.5) {
        t = 2.0 * value;
    } else  {
        t = 2.0 * (value - 0.5);
        blend_color_0 = color_1;
        blend_color_1 = color_2;
    }
    vec4 color;
    color.a = alpha;
    color.rgb = blend_color_0 * (1.0 - t) + blend_color_1 * t;
    return color;
}

/**
 * Intersects a ray with the bounding box and sets the intersection points.
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

/**
 * Find and set the current ray and bounding box from the input.
 */
void findPosition(vec3 viewport, out BoundingBox bounding_box, out Ray ray)
{
    vec3 relative_pos = vec3(2. * (gl_FragCoord.xy - viewport.xy) / vec2(viewport.z, viewport.z) - 1., input_bounding_box[2].z + 2);
    relative_pos = vec3(model_view_matrix * vec4(relative_pos, 1.));

    ray.origin = vec3(model_view_matrix * vec4(input_bounding_box[2], 1.));
    ray.direction = normalize(relative_pos - ray.origin);

    bounding_box.min = input_bounding_box[0];
    bounding_box.max = input_bounding_box[1];
}
