#ifndef RENDERVIEW_H
#define RENDERVIEW_H

#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLWidget>

#include "drawing/overlay_painter.h"
#include "util/tree_draw_properties.h"
#include "util/grid_controller.h"

/**
 * @brief The RenderView class Main rendering widget. Draws the grid and overlay based on draw properties which it takes ownership of.
 */
class RenderView : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT;

    TreeDrawProperties *draw_properties;

    GridController *grid_controller;
    QOpenGLDebugLogger debug_logger;

public:
    RenderView(QWidget *parent, TreeDrawProperties *draw_properties, GridController *grid_controller);
    ~RenderView();

public slots:
    void onGLMessageLogged(QOpenGLDebugMessage message);
    void repaint();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
};

#endif // RENDERVIEW_H
