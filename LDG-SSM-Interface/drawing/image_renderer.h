#ifndef IMAGE_RENDERER_H
#define IMAGE_RENDERER_H

#include <QOpenGLBuffer>
#include <QOpenGLTexture>

#include "renderer.h"

/**
 * @brief The ImageRenderer class Render class for rendering 2D image grids in OpenGL.
 */
class ImageRenderer : public Renderer
{
    QOpenGLTexture texture_array;

    GLint model_view_projection_uniform;
    GLuint vertex_array_object = 5;
    GLuint vertex_buffer, texcoord_buffer, index_buffer;

public:
    ImageRenderer(TreeDrawProperties *draw_properties);
    ~ImageRenderer();

    void intialize(QOpenGLFunctions_4_1_Core *gl) override;
    void updateBuffers() override;
    void updateUniforms() override;
    void render() override;
};

#endif // IMAGE_RENDERER_H
