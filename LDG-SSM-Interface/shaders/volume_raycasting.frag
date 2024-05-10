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
 * TODO: Make interactive in some way
 *
 * @param value The sample value
 * @return The color for the given sample value
 */
vec4 transferFunction(float value)
{
    vec4 color = vec4(0.);

    if (value >= 0.0 && value < 0.1) color = vec4(0.19483, 0.08339, 0.26149, value * 0.05);

    if (value >= 0.1 && value < 0.2) color = vec4(0.27648, 0.48144, 0.95064, value * 0.05);

    if (value >= 0.2 && value < 0.3) color = vec4(0.96187, 0.41093, 0.09310, value * 0.05);

    if (value >= 0.3 && value < 0.4) color = vec4(0.49321, 0.01963, 0.00955, value * 0.05);

    if (value >= 0.4 && value < 0.5) color = vec4(0.11167, 0.80569, 0.84525, value * 0.05);

    if (value >= 0.5 && value < 0.6) color = vec4(0.12733, 0.91701, 0.67627, value * 0.05);

    if (value >= 0.6 && value < 0.7) color = vec4(0.63323, 0.99195, 0.23937, value * 0.05);

    if (value >= 0.7 && value < 0.8) color = vec4(0.99438, 0.66386, 0.19971, value * 0.05);

    if (value >= 0.8 && value < 0.9) color = vec4(0.86079, 0.22945, 0.02875, value * 0.05);

    if (value >= 0.9 && value < 1.0) color = vec4(0.57103, 0.04474, 0.00529, value * 0.05);

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
