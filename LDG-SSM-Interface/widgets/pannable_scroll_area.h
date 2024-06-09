#ifndef PANNABLESCROLLAREA_H
#define PANNABLESCROLLAREA_H

#include <QOpenGLWidget>
#include <QScrollArea>

#include <drawing/model/window_draw_properties.h>

/**
 * A scroll area that is able to zoom and pan on the nested content.
 *
 * @brief The PannableScrollArea class
 */
class PannableScrollArea : public QScrollArea
{
    const double SCALE_STEP_SIZE = 0.1;

    WindowDrawProperties *window_properties;

    void resizeWidget();
    void updateViewport();

public:
    PannableScrollArea(QWidget *parent = nullptr);

    void setWindowDrawProperties(WindowDrawProperties *window_properties);

public slots:
    void fitWindow();
    void zoomIn();
    void zoomOut();
    void scrollContentsBy(int dx, int dy) override;

signals:
    void viewPortChanged(QRect viewport);

protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // PANNABLESCROLLAREA_H
