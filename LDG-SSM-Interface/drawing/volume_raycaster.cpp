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
    // gl->glDeleteBuffers(1, &volume_buffer);
    // gl->glDeleteBuffers(1, &projection_buffer);
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
    gl->glGenVertexArrays(1, &vertex_array_object);
    gl->glBindVertexArray(vertex_array_object);

    gl->glGenBuffers(1, &vertex_buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Because it uses a mat4 it needs 4 pointers...
    gl->glGenBuffers(1, &transformation_buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, transformation_buffer);
    for (unsigned int idx = 0; idx < 4; ++idx) {
        gl->glEnableVertexAttribArray(1 + idx);
        gl->glVertexAttribPointer(1 + idx, 4, GL_FLOAT, GL_FALSE, sizeof(QMatrix4x4), (const GLvoid *)(sizeof(QVector4D) * idx));
        gl->glVertexAttribDivisor(1 + idx, 1);
    }

    // gl->glGenBuffers(1, &volume_buffer);
    // gl->glBindBuffer(GL_ARRAY_BUFFER, volume_buffer);
    // gl->glEnableVertexAttribArray(2);
    // gl->glVertexAttribPointer(2, 256 * 256 * 256, GL_UNSIGNED_INT, GL_FALSE, 0, nullptr); // TODO: set proper size once data is known
    // gl->glVertexAttribDivisor(2, 1);   // Instanced

    // gl->glGenBuffers(1, &projection_buffer);
    // gl->glBindBuffer(GL_ARRAY_BUFFER, projection_buffer);
    // gl->glEnableVertexAttribArray(3);
    // gl->glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    // gl->glVertexAttribDivisor(3, 1);   // Instanced

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
    QList<unsigned int> volume_data(256 * 256 * 256, 0.);
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

    // gl->glBindBuffer(GL_ARRAY_BUFFER, volume_buffer);
    // gl->glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * volume_data.size(), volume_data.data(), GL_STATIC_DRAW);

    // gl->glBindBuffer(GL_ARRAY_BUFFER, projection_buffer);
    // gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * projection_origins.size(), projection_origins.data(), GL_STATIC_DRAW);
}

/**
 * @brief VolumeRaycaster::updateUniforms
 */
void VolumeRaycaster::updateUniforms()
{
    shader.bind();

    model_view_projection_uniform = shader.uniformLocation("model_view_projection_matrix");
    auto matrix = draw_properties->projection * draw_properties->model_view;
    gl->glUniformMatrix4fv(model_view_projection_uniform, 1, false, matrix.data());

    screen_space_projection = shader.uniformLocation("screen_space_projection");
    auto vector = draw_properties->gl_space_scale_vector;
    gl->glUniform3f(screen_space_projection, vector.x(), vector.y(), vector.z());

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

    gl->glBindVertexArray(vertex_array_object);
    gl->glDrawElementsInstanced(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, nullptr, draw_properties->draw_array.size());
    gl->glBindVertexArray(0);

    shader.release();
}


