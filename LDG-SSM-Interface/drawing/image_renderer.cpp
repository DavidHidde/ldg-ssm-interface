#include "image_renderer.h"
#include "drawing/model/mesh.h"

/**
 * @brief ImageRenderer::ImageRenderer
 * @param tree_properties
 * @param window_properties
 * @param texture_map
 */
ImageRenderer::ImageRenderer(TreeDrawProperties *tree_properties, WindowDrawProperties *window_properties):
    texture_array(QOpenGLTexture::Target2DArray),
    Renderer(tree_properties, window_properties)
{
}

/**
 * @brief ImageRenderer::~ImageRenderer
 */
ImageRenderer::~ImageRenderer()
{
    gl->glDeleteVertexArrays(1, &vertex_array_object);
    gl->glDeleteBuffers(1, &vertex_buffer);
    gl->glDeleteBuffers(1, &texcoord_buffer);
    gl->glDeleteBuffers(1, &texcoord_origin_buffer);
    gl->glDeleteBuffers(1, &transformation_buffer);
    gl->glDeleteBuffers(1, &index_buffer);

    vertex_array_object = 0;
    vertex_buffer = 0;
    texcoord_buffer = 0;
    transformation_buffer = 0;
    index_buffer = 0;

    texture_array.destroy();
}

/**
 * @brief ImageRenderer::intialize Initialize all data that should be initialized beforehand, like shaders and matrices.
 * @param gl
 */
void ImageRenderer::intialize(QOpenGLFunctions_4_1_Core *gl)
{
    this->gl = gl;

    gl->glEnable(GL_TEXTURE_2D_ARRAY);

    // Initialize matrices;
    tree_properties->projection.setToIdentity();
    tree_properties->projection.ortho(-1, 1, 1, -1, -1, 1);

    initializeBuffers();
    initializeShaders();
    initializeTextures();

    updateBuffers();
    updateUniforms();
}

/**
 * @brief ImageRenderer::initializeBuffers Initialize VAO and buffers.
 */
void ImageRenderer::initializeBuffers()
{
    gl->glGenVertexArrays(1, &vertex_array_object);
    gl->glBindVertexArray(vertex_array_object);

    gl->glGenBuffers(1, &vertex_buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    gl->glGenBuffers(1, &texcoord_buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, texcoord_buffer);
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    gl->glGenBuffers(1, &texcoord_origin_buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, texcoord_origin_buffer);
    gl->glEnableVertexAttribArray(2);
    gl->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    gl->glVertexAttribDivisor(2, 1); // Instanced

    // Mat4 requires 4 vertex attribute pointers
    gl->glGenBuffers(1, &transformation_buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, transformation_buffer);
    for (unsigned int idx = 0; idx < 4; ++idx) {
        gl->glEnableVertexAttribArray(3 + idx);
        gl->glVertexAttribPointer(3 + idx, 4, GL_FLOAT, GL_FALSE, sizeof(QMatrix4x4), (const GLvoid *)(sizeof(QVector4D) * idx));
        gl->glVertexAttribDivisor(3 + idx, 1); // Instanced
    }

    gl->glGenBuffers(1, &index_buffer);
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

    gl->glBindVertexArray(0);
}

/**
 * @brief ImageRenderer::initializeShaders Initialize the shader program.
 */
void ImageRenderer::initializeShaders()
{
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/image.vert");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/image.frag");
    shader.link();
}

/**
 * @brief ImageRenderer::initializeTextures Initialize the texture atlasses and texture array. We can keep these in memory.
 */
void ImageRenderer::initializeTextures()
{
    GLint max_texture_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    atlas_container = createImageAtlasContainer(tree_properties, max_texture_size);
    size_t num_atlasses = atlas_container.dims[2];

    texture_array.setMagnificationFilter(QOpenGLTexture::Linear);
    texture_array.setWrapMode(QOpenGLTexture::ClampToEdge);
    texture_array.setAutoMipMapGenerationEnabled(true);

    texture_array.setLayers(num_atlasses);
    texture_array.setSize(atlas_container.dims[0], atlas_container.dims[1]);
    texture_array.setFormat(QOpenGLTexture::RGBA8_UNorm);
    texture_array.allocateStorage();

    // Load the atlasses
    auto *data_ptr = atlas_container.data.data();
    size_t size_per_atlas = atlas_container.data.size() / num_atlasses;
    for (size_t idx = 0; idx < num_atlasses; ++idx) {
        texture_array.setData(0, idx, QOpenGLTexture::BGRA, QOpenGLTexture::UInt8, data_ptr + idx * size_per_atlas);
    }
}

/**
 * @brief ImageRenderer::updateBuffers Update the data buffers. We calculate the position of the vertices in screen space and project them into world space.
 */
void ImageRenderer::updateBuffers()
{
    // Set the base to-be-instanced shape, texture coords and indices
    double base_side_len = window_properties->height_node_lens[tree_properties->tree_max_height] * window_properties->device_pixel_ratio;
    auto mesh = createPlane(
        QVector3D{ 0., 0., 0. },
        base_side_len
    );
    gl->glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);

    gl->glBindBuffer(GL_ARRAY_BUFFER, texcoord_buffer);
    QList<QVector3D> texcoords{
        { 0.f, 0.f, 0.f },
        { atlas_container.coord_offsets.x(), 0.f, 0.f },
        { 0.f, atlas_container.coord_offsets.y(), 0.f },
        { atlas_container.coord_offsets.x(), atlas_container.coord_offsets.y(), 0.f }
    };
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * texcoords.size(), texcoords.data(), GL_STATIC_DRAW);

    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.indices.size(), mesh.indices.data(), GL_STATIC_DRAW);
    num_indices = mesh.indices.size();

    // Set data per instance
    QList<QVector3D> texcoords_origins;
    QList<QMatrix4x4> transformation_matrices;

    float spacing = window_properties->device_pixel_ratio * window_properties->node_spacing;
    for (auto &[height, index] : tree_properties->draw_array) {
        float side_len = window_properties->height_node_lens[height] * window_properties->device_pixel_ratio;
        auto [num_rows, num_cols] = tree_properties->height_dims[height];
        double x = index % num_cols;
        double y = index / num_cols;

        // Transformation translates to the origin of the cell and then scales down to the appropriates sizes.
        QMatrix4x4 transformation;
        QVector3D origin{
            static_cast<float>(x * (side_len + spacing)),
            static_cast<float>(y * (side_len + spacing)),
            0.
        };
        float factor = side_len / base_side_len;
        transformation.translate(origin);
        transformation.scale(factor, factor);
        transformation_matrices.append(transformation);

        // For the texture coordinate, we can simply pass the origin of the current texture to translate in the shader
        texcoords_origins.append(atlas_container.mapping[{ height, index }]);
    }

    // Bind and set data
    gl->glBindBuffer(GL_ARRAY_BUFFER, texcoord_origin_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * texcoords_origins.size(), texcoords_origins.data(), GL_STATIC_DRAW);

    gl->glBindBuffer(GL_ARRAY_BUFFER, transformation_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QMatrix4x4) * transformation_matrices.size(), transformation_matrices.data(), GL_STATIC_DRAW);
}

/**
 * @brief ImageRenderer::updateUniforms
 */
void ImageRenderer::updateUniforms()
{
    shader.bind();

    model_view_projection_uniform = shader.uniformLocation("projection_matrix");
    gl->glUniformMatrix4fv(model_view_projection_uniform, 1, false, tree_properties->projection.data());

    screen_origin_uniform = shader.uniformLocation("screen_origin");
    auto origin_vector = window_properties->device_pixel_ratio * window_properties->draw_origin;
    gl->glUniform2f(screen_origin_uniform, origin_vector.x(), origin_vector.y());

    screen_space_projection_uniform = shader.uniformLocation("screen_space_projection");
    auto &scale_vector = tree_properties->gl_space_scale_vector;
    gl->glUniform3f(screen_space_projection_uniform, scale_vector.x(), scale_vector.y(), scale_vector.z());

    shader.release();
}

/**
 * @brief ImageRenderer::render Actual draw call, where the objects need to be rendered.
 */
void ImageRenderer::render()
{
    gl->glEnable(GL_DEPTH_TEST);
    gl->glDepthFunc(GL_LEQUAL);

    shader.bind();
    texture_array.bind();
    gl->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    gl->glBindVertexArray(vertex_array_object);
    gl->glDrawElementsInstanced(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, nullptr, tree_properties->draw_array.size());
    gl->glBindVertexArray(0);

    texture_array.release();
    shader.release();
}
