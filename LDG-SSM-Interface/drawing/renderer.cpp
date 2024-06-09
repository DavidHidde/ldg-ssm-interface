#include "renderer.h"

/**
 * @brief Renderer::Renderer
 * @param tree_properties
 * @param window_properties
 */
Renderer::Renderer(TreeDrawProperties *tree_properties, WindowDrawProperties *window_properties):
    tree_properties(tree_properties),
    window_properties(window_properties)
{
}

/**
 * @brief Renderer::~Renderer
 */
Renderer::~Renderer()
{
}

/**
 * @brief Renderer::intialize
 * @param gl
 */
void Renderer::intialize(QOpenGLFunctions_4_1_Core *gl)
{
}

/**
 * @brief Renderer::updateBuffers
 */
void Renderer::updateBuffers()
{
}

/**
 * @brief Renderer::updateUniforms
 */
void Renderer::updateUniforms()
{
}

/**
 * @brief Renderer::render
 */
void Renderer::render()
{
}
