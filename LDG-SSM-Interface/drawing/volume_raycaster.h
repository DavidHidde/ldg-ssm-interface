#ifndef VOLUME_RAYCASTER_H
#define VOLUME_RAYCASTER_H

#include "renderer.h"

#include <QOpenGLTexture>

#include <drawing/model/volume_draw_properties.h>

#include <util/atlas_container.h>

/**
 * @brief The VolumeRaycaster class Renderer for performing volume raycasting
 */
class VolumeRaycaster : public Renderer
{
    VolumeDrawProperties *volume_properties;
    QMap<VolumeRenderingType, QOpenGLShaderProgram *> shaders;

    GLint projection_matrix_uniform, model_view_uniform, screen_space_projection_uniform, texture_coords_offset_uniform, bounding_box_uniform, num_samples_uniform, threshold_uniform;
    GLint background_color_uniform;

    GLuint vertex_array_object;
    GLuint vertex_buffer, transformation_buffer, viewport_buffer, texture_coords_buffer, index_buffer;

    AtlasContainer atlas_container;
    QOpenGLTexture volume_texture;

    size_t num_indices;

    void initializeBuffers();
    void initializeShaders();
    void initializeTexture();

public:
    VolumeRaycaster(TreeDrawProperties *draw_properties, WindowDrawProperties *window_properties, VolumeDrawProperties *volume_properties);
    ~VolumeRaycaster() override;

    void intialize(QOpenGLFunctions_4_1_Core *gl) override;

    void updateBuffers() override;
    void updateUniforms() override;
    void render() override;
};

#endif // VOLUME_RAYCASTER_H
