#ifndef PANNABLESCROLLAREA_H
#define PANNABLESCROLLAREA_H

#include <QOpenGLWidget>
#include <QScrollArea>

#include <drawing/model/tree_draw_properties.h>
#include <drawing/model/window_draw_properties.h>

#include <util/screen_controller.h>

/**
 * A scroll area that is able to zoom and interact with the underlying content.
 * This acts as a controller for the window itself, setting all screen-space related properties while delegating click-related interactions to the screen controller.
 *
 * @brief The PannableScrollArea class
 */
class PannableScrollArea : public QScrollArea
{
    Q_OBJECT;

    const double SCALE_STEP_SIZE = 0.1;

    WindowDrawProperties *window_properties;
    TreeDrawProperties *tree_properties;
    ScreenController *screen_controller;

    void resizeWidget();
    bool isReady();
    bool isInitialized();

public:
    PannableScrollArea(QWidget *parent = nullptr);

    void intialize(WindowDrawProperties *window_properties, TreeDrawProperties *tree_properties, ScreenController *screen_controller);
    void updateViewport();
    void updateWindowProperties();

protected:
    void resizeEvent(QResizeEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    bool viewportEvent(QEvent* event) override;

public slots:
    void fitWindow();
    void zoomIn();
    void zoomOut();
    void scrollContentsBy(int dx, int dy) override;

    void screenChanged();

signals:
    void viewportPositionChanged();
    void viewportSizeChanged();
};

#endif // PANNABLESCROLLAREA_H
