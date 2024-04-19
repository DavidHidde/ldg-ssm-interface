#ifndef IMAGE_RENDERER_H
#define IMAGE_RENDERER_H

#include <QOpenGLBuffer>
#include <QOpenGLTexture>

#include "util/image_atlas_container.h"
#include "renderer.h"

/**
 * @brief The ImageRenderer class Render class for rendering 2D image grids in OpenGL.
 */
class ImageRenderer : public Renderer
{
    QOpenGLTexture texture_array;

    GLint model_view_projection_uniform;
    GLuint vertex_array_object;
    GLuint vertex_buffer, texcoord_buffer, texindex_buffer, index_buffer;

    QMap<QPair<size_t, size_t>, QPair<QImage, double>> *image_data;
    ImageAtlasContainer atlas_container;

public:
    ImageRenderer(TreeDrawProperties *draw_properties, QMap<QPair<size_t, size_t>, QPair<QImage, double>> *image_data);
    ~ImageRenderer() override;

    void intialize(QOpenGLFunctions_4_1_Core *gl) override;
    void initializeBuffers();
    void initializeShaders();
    void initializeTextures();

    void updateBuffers() override;
    void updateUniforms() override;
    void render() override;
};

#endif // IMAGE_RENDERER_H
