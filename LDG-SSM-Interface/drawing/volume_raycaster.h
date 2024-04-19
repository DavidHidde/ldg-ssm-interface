#ifndef VOLUME_RAYCASTER_H
#define VOLUME_RAYCASTER_H

#include "renderer.h"

/**
 * @brief The VolumeRaycaster class Renderer for performing volume raycasting
 */
class VolumeRaycaster : public Renderer
{
    GLint model_view_projection_uniform;
    GLuint vertex_array_object;
    GLuint vertex_buffer, projection_index_buffer, index_buffer;

    size_t num_indices;

public:
    VolumeRaycaster(TreeDrawProperties *draw_properties);
    ~VolumeRaycaster() override;

    void intialize(QOpenGLFunctions_4_1_Core *gl) override;
    void initializeBuffers();
    void initializeShaders();

    void updateBuffers() override;
    void updateUniforms() override;
    void render() override;
};

#endif // VOLUME_RAYCASTER_H
