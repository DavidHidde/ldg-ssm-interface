#ifndef IMAGE_RENDERER_H
#define IMAGE_RENDERER_H

#include <QOpenGLBuffer>
#include <QOpenGLTexture>

#include "util/atlas_container.h"
#include "renderer.h"

/**
 * @brief The ImageRenderer class Render class for rendering 2D image grids in OpenGL.
 */
class ImageRenderer : public Renderer
{    
    QOpenGLTexture texture_array;
    QOpenGLShaderProgram shader;

    GLint model_view_projection_uniform, screen_space_projection_uniform, screen_origin_uniform;
    GLuint vertex_array_object;
    GLuint vertex_buffer, texcoord_buffer, texcoord_origin_buffer, transformation_buffer, index_buffer;

    AtlasContainer atlas_container;

    size_t num_indices;

    void initializeBuffers();
    void initializeShaders();
    void initializeTextures();

public:
    ImageRenderer(TreeDrawProperties *tree_properties, WindowDrawProperties *window_properties);
    ~ImageRenderer() override;

    void intialize(QOpenGLFunctions_4_1_Core *gl) override;

    void updateBuffers() override;
    void updateUniforms() override;
    void render() override;
};

#endif // IMAGE_RENDERER_H
