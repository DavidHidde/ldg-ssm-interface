#ifndef RENDERER_H
#define RENDERER_H

#include <QObject>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLShaderProgram>

#include "drawing/model/tree_draw_properties.h"

/**
 * @brief The Renderer class Interface for render classes.
 */
class Renderer : public QObject
{
    Q_OBJECT

public:
    Renderer(TreeDrawProperties *tree_properties);
    virtual ~Renderer();

    virtual void intialize(QOpenGLFunctions_4_1_Core *gl);
    virtual void updateBuffers();
    virtual void updateUniforms();
    virtual void render();

protected:
    QOpenGLFunctions_4_1_Core *gl;
    TreeDrawProperties *tree_properties;
};

#endif // RENDERER_H
