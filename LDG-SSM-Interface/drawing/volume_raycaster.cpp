#include "volume_raycaster.h"
#include "drawing/model/mesh.h"

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
    gl->glDeleteBuffers(1, &projection_index_buffer);
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

    gl->glGenBuffers(1, &projection_index_buffer);
    gl->glBindBuffer(GL_ARRAY_BUFFER, projection_index_buffer);
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 1, GL_UNSIGNED_INT, GL_FALSE, 0, nullptr);

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
    // Gen data
    QList<QVector3D> vertices;
    QList<unsigned int> projection_indices;
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
            0
        };
        auto mesh = createCube(origin, side_len, draw_properties->gl_space_scale_vector, counter);
        vertices.append(mesh.vertices);
        indices.append(mesh.indices);
        counter += mesh.vertices.size();

        // TODO: Do something for projection
        for (size_t idx = 0; idx < 8; ++idx) {
            projection_indices.append(0);
        }
    }

    // Bind and set data
    gl->glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    gl->glBindBuffer(GL_ARRAY_BUFFER, projection_index_buffer);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * projection_indices.size(), projection_indices.data(), GL_STATIC_DRAW);

    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
    num_indices = indices.size();
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
    gl->glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, nullptr);
    gl->glBindVertexArray(0);

    shader.release();
}


