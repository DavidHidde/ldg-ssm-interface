#include "image_renderer.h"

/**
 * @brief ImageRenderer::ImageRenderer
 * @param draw_properties
 */
ImageRenderer::ImageRenderer(TreeDrawProperties *draw_properties):
    texture_array(QOpenGLTexture::Target2D),
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
    gl->glDeleteBuffers(1, &index_buffer);

    texture_array.destroy();
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

    // Initialize shader
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/image.vert");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/image.frag");
    shader.link();

    // Initialize texture
    texture_array.setMagnificationFilter(QOpenGLTexture::Linear);
    texture_array.setWrapMode(QOpenGLTexture::ClampToEdge);
    texture_array.setAutoMipMapGenerationEnabled(true);

    // Initialize VAO and buffers
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

    gl->glGenBuffers(1, &index_buffer);
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

    gl->glBindVertexArray(0);

    updateBuffers();
    updateUniforms();
}

/**
 * @brief ImageRenderer::updateBuffers Update the data buffers. We calculate the position of the vertices in screen space and project them into world space.
 */
void ImageRenderer::updateBuffers()
{
    // Figure out image dims to keep aspect ratio
    QImage image{":/doot.jpg" };    // TODO: replace with actual image and calcualte beforehand
    float im_width = image.width();
    float im_height = image.height();
    float max_dim = std::max(im_width, im_height);
    float left_x = (im_width - max_dim) / (2. * im_width);
    float top_y = (im_height - max_dim) / (2. * im_height);
    float right_x = -left_x + 1.;
    float bot_y = -top_y + 1.;

    // Gen data
    QList<QVector3D> vertices;
    QList<QVector2D> texcoords;
    QList<unsigned int> indices;

    unsigned int counter = 0;
    for (auto [height, index] : draw_properties->draw_array) {
        float side_len = draw_properties->height_node_lens[height];
        auto [num_rows, num_cols] = draw_properties->height_dims[height];
        double x = index % num_cols;
        double y = index / num_cols;

        QVector3D origin{
            static_cast<float>(x * (side_len + draw_properties->node_spacing)),
            static_cast<float>(y * (side_len + draw_properties->node_spacing)),
            -1.
        };

        // Top left - 0
        vertices.append(origin * draw_properties->gl_space_scale_vector - QVector3D{ 1., 1., 0.});
        texcoords.append( { left_x, top_y } );
        // Top right - 1
        vertices.append((origin + QVector3D{ side_len, 0., 0. }) * draw_properties->gl_space_scale_vector - QVector3D{ 1., 1., 0.});
        texcoords.append( { right_x, top_y } );
        // Bot left - 2
        vertices.append((origin + QVector3D{ 0., side_len, 0. }) * draw_properties->gl_space_scale_vector - QVector3D{ 1., 1., 0.});
        texcoords.append( { left_x, bot_y } );
        // Bot right - 3
        vertices.append((origin + QVector3D{ side_len, side_len, 0. }) * draw_properties->gl_space_scale_vector - QVector3D{ 1., 1., 0.});
        texcoords.append( { right_x, bot_y } );

        indices.append(counter);
        indices.append(counter + 2);
        indices.append(counter + 3);
        indices.append(counter);
        indices.append(counter + 1);
        indices.append(counter + 3);
        counter += 4;
    }

    // Bind and set data
    gl->glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    gl->glBindBuffer(GL_ARRAY_BUFFER, texcoord_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector2D) * texcoords.size(), texcoords.data(), GL_STATIC_DRAW);

    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // Bind and set texture TODO: Make array and use multiple textures.
    texture_array.destroy();
    texture_array.create();
    texture_array.setData(QImage{":/doot.jpg" });
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
    updateUniforms();

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
