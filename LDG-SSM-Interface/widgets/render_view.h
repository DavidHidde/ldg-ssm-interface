#ifndef RENDERVIEW_H
#define RENDERVIEW_H

#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLWidget>

#include "drawing/renderer.h"
#include "drawing/model/tree_draw_properties.h"

/**
 * @brief The RenderView class Main rendering widget. Orchestrates the drawing of the grid and overlay based on draw properties. This is purely a view.
 */
class RenderView : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT;

    TreeDrawProperties *tree_properties;
    WindowDrawProperties *window_properties;

    Renderer *renderer;
    QOpenGLDebugLogger debug_logger;
    QOpenGLFunctions_4_1_Core *gl;

public:
    RenderView(
        QWidget *parent,
        TreeDrawProperties *draw_properties,
        WindowDrawProperties *window_properties
    );
    ~RenderView();

    void setRenderer(Renderer *renderer);
    void deleteRenderer();

public slots:
    void onGLMessageLogged(QOpenGLDebugMessage message);
    void updateBuffers();
    void updateUniforms();

private slots:
    void cleanup();

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
};

#endif // RENDERVIEW_H
