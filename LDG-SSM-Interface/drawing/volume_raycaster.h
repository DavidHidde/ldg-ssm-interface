#ifndef VOLUME_RAYCASTER_H
#define VOLUME_RAYCASTER_H

#include "renderer.h"

#include <QOpenGLTexture>

/**
 * @brief The VolumeRaycaster class Renderer for performing volume raycasting
 */
class VolumeRaycaster : public Renderer
{
    GLint projection_matrix_uniform, model_view_uniform, screen_space_projection_uniform, bounding_box_uniform;

    GLuint vertex_array_object;
    GLuint vertex_buffer, transformation_buffer, index_buffer;

    QOpenGLTexture volume_texture;

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
