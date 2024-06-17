#include "volume_raycaster.h"
#include "drawing/model/mesh.h"

#include <QOpenGLPixelTransferOptions>

/**
 * @brief VolumeRaycaster::VolumeRaycaster
 * @param tree_properties
 * @param window_properties
 * @param volume_properties
 */
VolumeRaycaster::VolumeRaycaster(TreeDrawProperties *tree_properties, WindowDrawProperties *window_properties, VolumeDrawProperties *volume_properties):
    volume_properties(volume_properties),
    volume_texture(QOpenGLTexture::Target3D),
    Renderer(tree_properties, window_properties)
{
}

/**
 * @brief VolumeRaycaster::~VolumeRaycaster
 */
VolumeRaycaster::~VolumeRaycaster()
{
    gl->glDeleteVertexArrays(1, &vertex_array_object);
    gl->glDeleteBuffers(1, &vertex_buffer);
    gl->glDeleteBuffers(1, &transformation_buffer);
    gl->glDeleteBuffers(1, &viewport_buffer);
    gl->glDeleteBuffers(1, &texture_coords_buffer);
    gl->glDeleteBuffers(1, &index_buffer);

    vertex_array_object = 0;
    vertex_buffer = 0;
    transformation_buffer = 0;
    viewport_buffer = 0;
    texture_coords_buffer = 0;
    index_buffer = 0;

    volume_texture.destroy();
    qDeleteAll(shaders);
}

/**
 * @brief VolumeRaycaster::intialize Initialize all data that should be initialized beforehand, like shaders and matrices.
 * @param gl
 */
void VolumeRaycaster::intialize(QOpenGLFunctions_4_1_Core *gl)
{
    this->gl = gl;

    // Initialize matrices;
    tree_properties->projection.setToIdentity();
    tree_properties->projection.ortho(-1, 1, 1, -1, 0, -20);

    initializeBuffers();
    initializeShaders();
    initializeTexture();

    updateBuffers();
    updateUniforms();
}

/**
 * @brief VolumeRaycaster::initializeBuffers Initialize VAO and buffers.
 */
void VolumeRaycaster::initializeBuffers()
{
    GLuint vertex_buf_loc = 0;
    GLuint viewport_buf_loc = 1;
    GLuint texture_coords_buf_loc = 2;
    GLuint transformation_buf_loc = 3;

    gl->glGenVertexArrays(1, &vertex_array_object);
    gl->glBindVertexArray(vertex_array_object);

    gl->glGenBuffers(1, &vertex_buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    gl->glEnableVertexAttribArray(vertex_buf_loc);
    gl->glVertexAttribPointer(vertex_buf_loc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    gl->glGenBuffers(1, &viewport_buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, viewport_buffer);
    gl->glEnableVertexAttribArray(viewport_buf_loc);
    gl->glVertexAttribPointer(viewport_buf_loc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    gl->glVertexAttribDivisor(viewport_buf_loc, 1); // Instanced

    gl->glGenBuffers(1, &texture_coords_buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, texture_coords_buffer);
    gl->glEnableVertexAttribArray(texture_coords_buf_loc);
    gl->glVertexAttribPointer(texture_coords_buf_loc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    gl->glVertexAttribDivisor(texture_coords_buf_loc, 1); // Instanced

    // Mat4 requires 4 vertex attribute pointers
    gl->glGenBuffers(1, &transformation_buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, transformation_buffer);
    for (unsigned int idx = 0; idx < 4; ++idx) {
        gl->glEnableVertexAttribArray(transformation_buf_loc + idx);
        gl->glVertexAttribPointer(transformation_buf_loc + idx, 4, GL_FLOAT, GL_FALSE, sizeof(QMatrix4x4), (const GLvoid *)(sizeof(QVector4D) * idx));
        gl->glVertexAttribDivisor(transformation_buf_loc + idx, 1); // Instanced
    }

    gl->glGenBuffers(1, &index_buffer);
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

    gl->glBindVertexArray(0);
    gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 * @brief VolumeRaycaster::initializeShaders Initialize the shader program
 */
void VolumeRaycaster::initializeShaders()
{
    QMap<QString, VolumeRenderingType> map{
        { "accumulate", VolumeRenderingType::ACCUMULATE },
        { "average_intensity", VolumeRenderingType::AVERAGE },
        { "isosurface", VolumeRenderingType::ISOSURFACE },
        { "max_intensity", VolumeRenderingType::MAX }
    };

    for (auto [file_name, render_type] : map.asKeyValueRange()) {
        QOpenGLShaderProgram *shader = new QOpenGLShaderProgram();
        shader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/volume_raycasting.vert");
        shader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/" + file_name + ".frag");
        shader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/volume_raycasting.frag");
        shader->link();
        shaders.insert(render_type, shader);
    }
}

/**
 * @brief VolumeRaycaster::initializeTexture Initialize the volume atlas.
 */
void VolumeRaycaster::initializeTexture()
{
    GLint max_texture_size;
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max_texture_size);
    atlas_container = createVolumeAtlasContainer(tree_properties, max_texture_size);

    volume_texture.setWrapMode(QOpenGLTexture::ClampToEdge);
    volume_texture.setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
    volume_texture.setSize(atlas_container.dims[0], atlas_container.dims[1], atlas_container.dims[2]);
    volume_texture.setFormat(QOpenGLTexture::R8_UNorm);
    volume_texture.allocateStorage();
    QOpenGLPixelTransferOptions transfer_options;
    transfer_options.setAlignment(1);
    volume_texture.setData(0, QOpenGLTexture::Red, QOpenGLTexture::UInt8, atlas_container.data.data(), &transfer_options);
}

/**
 * @brief VolumeRaycaster::updateBuffers Put the appropriate volumes into memory
 */
void VolumeRaycaster::updateBuffers()
{
    // Set the base to-be-instanced shape and indices
    double base_side_len = window_properties->height_node_lens[tree_properties->tree_max_height] * window_properties->device_pixel_ratio;
    auto mesh = createCube(
        QVector3D{ 0., 0., 0. },
        base_side_len,
        -1
    );
    gl->glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.indices.size(), mesh.indices.data(), GL_STATIC_DRAW);
    num_indices = mesh.indices.size();

    // Set data per instance
    QList<QMatrix4x4> transformation_matrices;
    QList<QVector3D> viewport_vectors;
    QList<QVector3D> volume_coords;
    float spacing = window_properties->node_spacing * window_properties->device_pixel_ratio;
    float window_height = window_properties->scaled_window_size.y() * window_properties->device_pixel_ratio;
    for (auto &[height, index] : tree_properties->draw_array) {
        float side_len = window_properties->height_node_lens[height] * window_properties->device_pixel_ratio;
        auto [num_rows, num_cols] = tree_properties->height_dims[height];
        double x = index % num_cols;
        double y = index / num_cols;

        // Transformation translates to the origin of the cell and then scales down to the appropriates sizes.
        // We remove 4 pixels from each side to deal with overdraw of the overlay.
        QMatrix4x4 transformation;
        QVector3D origin{
            static_cast<float>(x * (side_len + spacing)) + 4,
            static_cast<float>(y * (side_len + spacing)) + 4,
            0.
        };
        float factor = (side_len - 8) / base_side_len;
        transformation.translate(origin);
        transformation.scale(factor, factor);
        transformation_matrices.append(transformation);

        // Viewport consists of the origin and side lengths
        viewport_vectors.append({
            origin.x(),
            origin.y(),
            side_len
        });

        // For the texture coordinates we just need to know the start of the texture
        volume_coords.append(atlas_container.mapping[{ height, index }]);
    }

    // Bind and set data
    gl->glBindBuffer(GL_ARRAY_BUFFER, transformation_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QMatrix4x4) * transformation_matrices.size(), transformation_matrices.data(), GL_STATIC_DRAW);

    gl->glBindBuffer(GL_ARRAY_BUFFER, viewport_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * viewport_vectors.size(), viewport_vectors.data(), GL_STATIC_DRAW);

    gl->glBindBuffer(GL_ARRAY_BUFFER, texture_coords_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * volume_coords.size(), volume_coords.data(), GL_STATIC_DRAW);
}

/**
 * @brief VolumeRaycaster::updateUniforms
 */
void VolumeRaycaster::updateUniforms()
{
    auto shader = shaders[volume_properties->render_type];
    shader->bind();

    model_view_uniform = shader->uniformLocation("model_view_matrix");
    gl->glUniformMatrix4fv(model_view_uniform, 1, false, volume_properties->camera_view_transformation.data());

    projection_matrix_uniform = shader->uniformLocation("projection_matrix");
    gl->glUniformMatrix4fv(projection_matrix_uniform, 1, false, tree_properties->projection.data());

    screen_origin_uniform = shader->uniformLocation("screen_origin");
    auto origin_vector = window_properties->device_pixel_ratio * window_properties->draw_origin;
    gl->glUniform2f(screen_origin_uniform, origin_vector.x(), origin_vector.y());

    screen_space_projection_uniform = shader->uniformLocation("screen_space_projection");
    auto vector = tree_properties->gl_space_scale_vector;
    gl->glUniform3f(screen_space_projection_uniform, vector.x(), vector.y(), vector.z());

    texture_coords_offset_uniform = shader->uniformLocation("texture_coords_offset");
    gl->glUniform3f(texture_coords_offset_uniform, atlas_container.coord_offsets.x(), atlas_container.coord_offsets.y(), atlas_container.coord_offsets.z());

    bounding_box_uniform = shader->uniformLocation("input_bounding_box");
    auto mesh = createCube(
        QVector3D{ -1., -1., -1. },
        2,
        1.
    );
    auto origin = mesh.vertices.first();
    auto end = mesh.vertices.last();
    auto center = (origin + end) / 2.;

    QMatrix3x3 bounding_box;
    bounding_box(0, 0) = std::min(origin.x(), end.x()); bounding_box(0, 1) = std::min(origin.y(), end.y()); bounding_box(0, 2) = std::min(origin.z(), end.z());
    bounding_box(1, 0) = std::max(origin.x(), end.x()); bounding_box(1, 1) = std::max(origin.y(), end.y()); bounding_box(1, 2) = std::max(origin.z(), end.z());
    bounding_box(2, 0) = center.x();                    bounding_box(2, 1) = center.y();                    bounding_box(2, 2) = -3.5;
    gl->glUniformMatrix3fv(bounding_box_uniform, 1, true, bounding_box.data());

    num_samples_uniform = shader->uniformLocation("num_samples");
    gl->glUniform1f(num_samples_uniform, volume_properties->sample_steps);

    background_color_uniform = shader->uniformLocation("background_color");
    gl->glUniform3f(background_color_uniform, tree_properties->background_color.x(), tree_properties->background_color.y(), tree_properties->background_color.z());

    if (volume_properties->render_type == VolumeRenderingType::ISOSURFACE) {
        threshold_uniform = shader->uniformLocation("threshold");
        gl->glUniform1f(threshold_uniform, volume_properties->threshold);
    }

    shader->release();
}

/**
 * @brief VolumeRaycaster::render Actual draw call, where the objects need to be rendered.
 */
void VolumeRaycaster::render()
{   
    gl->glEnable(GL_DEPTH_TEST);
    gl->glDepthFunc(GL_LEQUAL);

    shaders[volume_properties->render_type]->bind();
    volume_texture.bind();
    gl->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    gl->glBindVertexArray(vertex_array_object);
    gl->glDrawElementsInstanced(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, nullptr, tree_properties->draw_array.size());
    gl->glBindVertexArray(0);

    volume_texture.release();
    shaders[volume_properties->render_type]->release();
}


