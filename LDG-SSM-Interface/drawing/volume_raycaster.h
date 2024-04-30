#ifndef VOLUME_RAYCASTER_H
#define VOLUME_RAYCASTER_H

#include "renderer.h"

#include <QOpenGLTexture>

#include <drawing/model/volume_draw_properties.h>

/**
 * @brief The VolumeRaycaster class Renderer for performing volume raycasting
 */
class VolumeRaycaster : public Renderer
{
    VolumeDrawProperties *volume_properties;
    QMap<VolumeRenderingType, QOpenGLShaderProgram *> shaders;

    GLint projection_matrix_uniform, model_view_uniform, screen_space_projection_uniform, bounding_box_uniform, num_samples_uniform;
    GLint background_color_uniform, color_0_uniform, color_1_uniform, color_2_uniform;

    GLuint vertex_array_object;
    GLuint vertex_buffer, transformation_buffer, viewport_buffer, index_buffer;

    QOpenGLTexture volume_texture;

    size_t num_indices;

    void initializeBuffers();
    void initializeShaders();

public:
    VolumeRaycaster(TreeDrawProperties *draw_properties, VolumeDrawProperties *volume_properties);
    ~VolumeRaycaster() override;

    void intialize(QOpenGLFunctions_4_1_Core *gl) override;

    void updateBuffers() override;
    void updateUniforms() override;
    void render() override;
};

#endif // VOLUME_RAYCASTER_H
