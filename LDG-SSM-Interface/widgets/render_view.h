#ifndef RENDERVIEW_H
#define RENDERVIEW_H

#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLWidget>

#include "drawing/renderer.h"
#include "drawing/model/tree_draw_properties.h"
#include "util/grid_controller.h"

/**
 * @brief The RenderView class Main rendering widget. Orchestrates the drawing of the grid and overlay based on draw properties which it takes ownership of.
 */
class RenderView : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT;

    TreeDrawProperties *tree_properties;
    WindowDrawProperties *window_properties;

    GridController *grid_controller;
    Renderer *renderer;
    QOpenGLDebugLogger debug_logger;

public:
    RenderView(
        QWidget *parent,
        TreeDrawProperties *draw_properties,
        WindowDrawProperties *window_properties,
        GridController *grid_controller,
        Renderer *renderer
    );
    ~RenderView();

public slots:
    void onGLMessageLogged(QOpenGLDebugMessage message);
    void updateBuffers();
    void updateUniforms();

    void resetView();
    void screenChanged();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
};

#endif // RENDERVIEW_H
