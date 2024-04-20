#ifndef VOLUME_RAYCASTER_H
#define VOLUME_RAYCASTER_H

#include "renderer.h"

/**
 * @brief The VolumeRaycaster class Renderer for performing volume raycasting
 */
class VolumeRaycaster : public Renderer
{
    GLint projection_matrix_uniform, model_view_uniform, screen_space_projection_uniform, volume_data_uniform;

    GLuint vertex_array_object;
    GLuint volume_texture_object;
    GLuint vertex_buffer, transformation_buffer, projection_buffer, volume_texture_buffer, index_buffer;

    size_t num_indices;

    void initializeBuffers();
    void initializeShaders();

public:
    VolumeRaycaster(TreeDrawProperties *draw_properties);
    ~VolumeRaycaster() override;

    void intialize(QOpenGLFunctions_4_1_Core *gl) override;

    void updateBuffers() override;
    void updateUniforms() override;
    void render() override;
};

#endif // VOLUME_RAYCASTER_H
