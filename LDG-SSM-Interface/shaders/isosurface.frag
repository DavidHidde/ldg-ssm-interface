#version 410

const vec3 material_color = vec3(1., 1., 1.);
const vec3 light_position = vec3(3., 0., -3.);

flat in vec3 texture_coord_start;
flat in vec3 viewport;

uniform sampler3D volume;

uniform mat4 model_view_matrix;

uniform vec3 texture_coords_offset;
uniform vec3 background_color;
uniform float num_samples;
uniform float threshold;

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


// Estimate the normal from a finite difference approximation of the gradient
vec3 normal(vec3 position, float intensity, float step_length)
{
    float d = step_length;
    float dx = texture(volume, texture_coord_start + (position + vec3(d, 0, 0)) * texture_coords_offset).r - intensity;
    float dy = texture(volume, texture_coord_start + (position + vec3(0, d, 0)) * texture_coords_offset).r - intensity;
    float dz = texture(volume, texture_coord_start + (position + vec3(0, 0, d)) * texture_coords_offset).r - intensity;
    return -normalize(vec3(dx, dy, dz));
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
    vec4 final_color = vec4(background_color, 1.);

    // Main raycasting loop
    float t = t_near;
    while(t < t_far) {
        // Normalize texture coordinates based on volume
        vec3 pos = (ray.origin + t * ray.direction - bounding_box.min) / (bounding_box.max - bounding_box.min);
        float value = texture(volume, texture_coord_start + pos * texture_coords_offset).r;

        if (value > threshold) {
            vec3 L = normalize(vec3(model_view_matrix * vec4(light_position, 1.)) - pos);
            vec3 V = -normalize(ray.direction);
            vec3 N = normal(pos, value, t_step);
            vec3 H = normalize(L + V);

            // Blinn-Phong shading
            float Ia = 0.1;
            float Id = 1.0 * max(0, dot(N, L));
            float Is = 8.0 * pow(max(0, dot(N, H)), 600);
            final_color = vec4((Ia + Id) * material_color + Is * vec3(1.0), 1.);

            break;
        }

        t += t_step;
    }
    frag_color = final_color;
}
