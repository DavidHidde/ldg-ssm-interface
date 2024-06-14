#ifndef PANNABLESCROLLAREA_H
#define PANNABLESCROLLAREA_H

#include <QOpenGLWidget>
#include <QScrollArea>

#include <drawing/model/tree_draw_properties.h>
#include <drawing/model/window_draw_properties.h>

#include <util/grid_controller.h>

/**
 * A scroll area that is able to zoom and interact with the underlying content. This acts as a controller for mouse interactions.
 *
 * @brief The PannableScrollArea class
 */
class PannableScrollArea : public QScrollArea
{
    const double SCALE_STEP_SIZE = 0.1;

    WindowDrawProperties *window_properties;
    TreeDrawProperties *tree_properties;
    GridController *grid_controller;

    void resizeWidget();
    void updateViewport();
    bool isReady();

public:
    PannableScrollArea(QWidget *parent = nullptr);

    void intialize(WindowDrawProperties *window_properties, TreeDrawProperties *tree_properties, GridController *grid_controller);
    void updateWindowProperties();

protected:
    void resizeEvent(QResizeEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

public slots:
    void fitWindow();
    void zoomIn();
    void zoomOut();
    void scrollContentsBy(int dx, int dy) override;

    void screenChanged();
};

#endif // PANNABLESCROLLAREA_H
