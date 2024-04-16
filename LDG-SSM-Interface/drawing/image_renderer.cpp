#include "image_renderer.h"

/**
 * @brief ImageRenderer::ImageRenderer
 * @param draw_properties
 * @param texture_map
 */
ImageRenderer::ImageRenderer(TreeDrawProperties *draw_properties, QMap<QPair<size_t, size_t>, QImage> *image_data):
    texture_array(QOpenGLTexture::Target2DArray),
    image_data(image_data),
    atlas_container({}, {}, {}),
    Renderer(draw_properties)
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
    gl->glDeleteBuffers(1, &texindex_buffer);
    gl->glDeleteBuffers(1, &index_buffer);

    texture_array.destroy();
    delete image_data;
}

/**
 * @brief ImageRenderer::intialize Initialize all data that should be initialized beforehand, like shaders and matrices.
 * @param gl
 */
void ImageRenderer::intialize(QOpenGLFunctions_4_1_Core *gl)
{
    this->gl = gl;

    // Enable depth buffer and culling
    gl->glEnable(GL_DEPTH_TEST);
    gl->glDepthFunc(GL_LEQUAL);
    gl->glEnable(GL_TEXTURE_2D_ARRAY);

    // Initialize matrices;
    draw_properties->projection.setToIdentity();
    draw_properties->model_view.setToIdentity();
    draw_properties->projection.ortho(-1, 1, 1, -1, -1, 1);

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
    gl->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    gl->glGenBuffers(1, &texindex_buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, texindex_buffer);
    gl->glEnableVertexAttribArray(2);
    gl->glVertexAttribPointer(2, 1, GL_UNSIGNED_INT, GL_FALSE, 0, nullptr);

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
    atlas_container = createAtlasContainer(image_data, max_texture_size);
    size_t num_atlasses = atlas_container.image_atlasses.size();

    texture_array.setMagnificationFilter(QOpenGLTexture::Linear);
    texture_array.setWrapMode(QOpenGLTexture::ClampToEdge);
    texture_array.setAutoMipMapGenerationEnabled(true);

    texture_array.setLayers(num_atlasses);
    texture_array.setSize(atlas_container.image_atlasses[0].width(), atlas_container.image_atlasses[0].height());
    texture_array.setFormat(QOpenGLTexture::RGBA8_UNorm);
    texture_array.allocateStorage();

    // Load the atlasses
    for (size_t idx = 0; idx < num_atlasses; ++idx) {
        auto &atlas = atlas_container.image_atlasses[idx];
        texture_array.setData(0, idx, QOpenGLTexture::BGRA, QOpenGLTexture::UInt8, atlas.constBits());
    }
}

/**
 * @brief ImageRenderer::updateBuffers Update the data buffers. We calculate the position of the vertices in screen space and project them into world space.
 */
void ImageRenderer::updateBuffers()
{
    // Gen data
    QList<QVector3D> vertices;
    QList<QVector2D> texcoords;
    QList<unsigned int> texindices;
    QList<unsigned int> indices;

    unsigned int counter = 0;
    for (auto &[height, index] : draw_properties->draw_array) {
        float side_len = draw_properties->height_node_lens[height];
        auto [num_rows, num_cols] = draw_properties->height_dims[height];
        double x = index % num_cols;
        double y = index / num_cols;

        QVector3D origin{
            static_cast<float>(x * (side_len + draw_properties->node_spacing)),
            static_cast<float>(y * (side_len + draw_properties->node_spacing)),
            -1.
        };
        // QVector3D cell_texcoords = atlas_container.mapping[{ height, index }];
        QVector3D cell_texcoords{
            index % 2 ? static_cast<float>(atlas_container.coord_offsets.x()) : 0.f,
            0.f,
            0.
        };

        // Top left - 0
        vertices.append(origin * draw_properties->gl_space_scale_vector - QVector3D{ 1., 1., 0.});
        texcoords.append( { cell_texcoords.x(), cell_texcoords.y() } );
        // Top right - 1
        vertices.append((origin + QVector3D{ side_len, 0., 0. }) * draw_properties->gl_space_scale_vector - QVector3D{ 1., 1., 0.});
        texcoords.append( { cell_texcoords.x() + atlas_container.coord_offsets.x(), cell_texcoords.y() } );
        // Bot left - 2
        vertices.append((origin + QVector3D{ 0., side_len, 0. }) * draw_properties->gl_space_scale_vector - QVector3D{ 1., 1., 0.});
        texcoords.append( { cell_texcoords.x(), cell_texcoords.y() + atlas_container.coord_offsets.y() } );
        // Bot right - 3
        vertices.append((origin + QVector3D{ side_len, side_len, 0. }) * draw_properties->gl_space_scale_vector - QVector3D{ 1., 1., 0.});
        texcoords.append( { cell_texcoords.x() + atlas_container.coord_offsets.x(), cell_texcoords.y() + atlas_container.coord_offsets.y() } );

        // Arrange triangles
        indices.append(counter);
        indices.append(counter + 2);
        indices.append(counter + 3);
        indices.append(counter);
        indices.append(counter + 1);
        indices.append(counter + 3);

        // Add and assign texture layer
        texindices.append(cell_texcoords.z());
        texindices.append(cell_texcoords.z());
        texindices.append(cell_texcoords.z());
        texindices.append(cell_texcoords.z());

        counter += 4;
    }

    // Bind and set data
    gl->glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    gl->glBindBuffer(GL_ARRAY_BUFFER, texcoord_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D) * texcoords.size(), texcoords.data(), GL_STATIC_DRAW);

    gl->glBindBuffer(GL_ARRAY_BUFFER, texindex_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * texindices.size(), texindices.data(), GL_STATIC_DRAW);

    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
}

/**
 * @brief ImageRenderer::updateUniforms
 */
void ImageRenderer::updateUniforms()
{
    shader.bind();

    model_view_projection_uniform = shader.uniformLocation("model_view_projection_matrix");
    auto matrix = draw_properties->projection * draw_properties->model_view;
    gl->glUniformMatrix4fv(model_view_projection_uniform, 1, false, matrix.data());

    shader.release();
}

/**
 * @brief ImageRenderer::render Actual draw call, where the objects need to be rendered.
 */
void ImageRenderer::render()
{
    gl->glEnable(GL_DEPTH_TEST);
    gl->glDepthFunc(GL_LEQUAL);

    gl->glClearColor(0.0, 0.0, 0.0, 1.0);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.bind();
    texture_array.bind();
    gl->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    gl->glBindVertexArray(vertex_array_object);
    gl->glDrawElements(GL_TRIANGLES, draw_properties->draw_array.size() * 6, GL_UNSIGNED_INT, nullptr);
    gl->glBindVertexArray(0);

    texture_array.release();
    shader.release();
}
