#include "volume_raycaster.h"
#include "drawing/model/mesh.h"

#include <QOpenGLTexture>

/**
 * @brief VolumeRaycaster::VolumeRaycaster
 * @param draw_properties
 */
VolumeRaycaster::VolumeRaycaster(TreeDrawProperties *draw_properties):
    Renderer(draw_properties)
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
    gl->glDeleteBuffers(1, &projection_buffer);
    gl->glDeleteBuffers(1, &volume_texture_buffer);
    gl->glDeleteBuffers(1, &index_buffer);
}

/**
 * @brief VolumeRaycaster::intialize Initialize all data that should be initialized beforehand, like shaders and matrices.
 * @param gl
 */
void VolumeRaycaster::intialize(QOpenGLFunctions_4_1_Core *gl)
{
    this->gl = gl;

    // Enable depth buffer and culling
    gl->glEnable(GL_DEPTH_TEST);
    gl->glDepthFunc(GL_LEQUAL);

    // Initialize matrices;
    draw_properties->projection.setToIdentity();
    draw_properties->model_view.setToIdentity();
    draw_properties->projection.ortho(-1, 1, 1, -1, 0, -20);

    initializeBuffers();
    initializeShaders();

    updateBuffers();
    updateUniforms();
}

/**
 * @brief VolumeRaycaster::initializeBuffers Initialize VAO and buffers.
 */
void VolumeRaycaster::initializeBuffers()
{
    GLuint vertex_buf_loc = 0;
    GLuint transformation_buf_loc = 1;
    GLuint projection_buf_loc = 5;

    gl->glGenVertexArrays(1, &vertex_array_object);
    gl->glBindVertexArray(vertex_array_object);

    gl->glGenBuffers(1, &vertex_buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    gl->glEnableVertexAttribArray(vertex_buf_loc);
    gl->glVertexAttribPointer(vertex_buf_loc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Mat4 requires 4 vertex attribute pointers
    gl->glGenBuffers(1, &transformation_buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, transformation_buffer);
    for (unsigned int idx = 0; idx < 4; ++idx) {
        gl->glEnableVertexAttribArray(transformation_buf_loc + idx);
        gl->glVertexAttribPointer(transformation_buf_loc + idx, 4, GL_FLOAT, GL_FALSE, sizeof(QMatrix4x4), (const GLvoid *)(sizeof(QVector4D) * idx));
        gl->glVertexAttribDivisor(transformation_buf_loc + idx, 1);
    }

    gl->glGenBuffers(1, &projection_buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, projection_buffer);
    gl->glEnableVertexAttribArray(projection_buf_loc);
    gl->glVertexAttribPointer(projection_buf_loc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    gl->glVertexAttribDivisor(projection_buf_loc, 1);   // Instanced

    gl->glGenBuffers(1, &volume_texture_buffer);
    gl->glBindBuffer(GL_TEXTURE_BUFFER, volume_texture_buffer);
    gl->glGenTextures(1, &volume_texture_object);

    gl->glGenBuffers(1, &index_buffer);
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

    gl->glBindVertexArray(0);
}

/**
 * @brief VolumeRaycaster::initializeShaders Initialize the shader program
 */
void VolumeRaycaster::initializeShaders()
{
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/volume_raycasting.vert");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/volume_raycasting.frag");
    shader.link();
}

/**
 * @brief VolumeRaycaster::updateBuffers Put the appropriate volumes into memory
 */
void VolumeRaycaster::updateBuffers()
{
    // Set the base to-be-instanced shape and indices
    double base_side_len = draw_properties->height_node_lens[draw_properties->tree_max_height];
    auto mesh = createCube(
        QVector3D{ 0., 0., 0. },
        base_side_len
    );
    gl->glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.indices.size(), mesh.indices.data(), GL_STATIC_DRAW);
    num_indices = mesh.indices.size();

    // Set data per instance
    QList<QMatrix4x4> transformation_matrices;
    QList<unsigned char> volume_data(256 * 256 * 256 * draw_properties->draw_array.size(), 0.);
    QList<QVector3D> projection_origins;

    for (auto &[height, index] : draw_properties->draw_array) {
        float side_len = draw_properties->height_node_lens[height];
        auto [num_rows, num_cols] = draw_properties->height_dims[height];
        double x = index % num_cols;
        double y = index / num_cols;

        QMatrix4x4 transformation;
        QVector3D origin{
            static_cast<float>(x * (side_len + draw_properties->node_spacing)),
            static_cast<float>(y * (side_len + draw_properties->node_spacing)),
            0.
        };
        float factor = side_len / base_side_len;
        transformation.translate(origin);
        transformation.scale(factor, factor);
        transformation_matrices.append(transformation);

        projection_origins.append(origin + QVector3D{static_cast<float>(side_len / 2.), static_cast<float>(side_len / 2.), 0.});

        // TODO: Add volume data
    }

    // Bind and set data
    gl->glBindBuffer(GL_ARRAY_BUFFER, transformation_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QMatrix4x4) * transformation_matrices.size(), transformation_matrices.data(), GL_STATIC_DRAW);

    gl->glBindBuffer(GL_ARRAY_BUFFER, projection_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * projection_origins.size(), projection_origins.data(), GL_STATIC_DRAW);

    gl->glBindBuffer(GL_TEXTURE_BUFFER, volume_texture_buffer);
    gl->glBufferData(GL_TEXTURE_BUFFER, sizeof(unsigned char) * volume_data.size(), volume_data.data(), GL_STATIC_DRAW);

}

/**
 * @brief VolumeRaycaster::updateUniforms
 */
void VolumeRaycaster::updateUniforms()
{
    shader.bind();

    model_view_uniform = shader.uniformLocation("model_view_matrix");
    gl->glUniformMatrix4fv(model_view_uniform, 1, false, draw_properties->model_view.data());

    projection_matrix_uniform = shader.uniformLocation("projection_matrix");
    gl->glUniformMatrix4fv(projection_matrix_uniform, 1, false, draw_properties->projection.data());

    screen_space_projection_uniform = shader.uniformLocation("screen_space_projection");
    auto vector = draw_properties->gl_space_scale_vector;
    gl->glUniform3f(screen_space_projection_uniform, vector.x(), vector.y(), vector.z());

    volume_data_uniform = shader.uniformLocation("volume_data");
    gl->glUniform1i(volume_data_uniform, 0);

    shader.release();
}

/**
 * @brief VolumeRaycaster::render Actual draw call, where the objects need to be rendered.
 */
void VolumeRaycaster::render()
{
    gl->glEnable(GL_DEPTH_TEST);
    gl->glDepthFunc(GL_LEQUAL);

    gl->glClearColor(0.0, 0.0, 0.0, 1.0);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.bind();
    gl->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(GL_TEXTURE_BUFFER, volume_texture_object);
    gl->glTexBuffer(GL_TEXTURE_BUFFER, GL_R8, volume_texture_buffer);

    gl->glBindVertexArray(vertex_array_object);
    gl->glDrawElementsInstanced(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, nullptr, draw_properties->draw_array.size());
    gl->glBindVertexArray(0);

    shader.release();
}


