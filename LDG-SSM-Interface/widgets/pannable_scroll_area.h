#ifndef PANNABLESCROLLAREA_H
#define PANNABLESCROLLAREA_H

#include <QOpenGLWidget>
#include <QScrollArea>

/**
 * A scroll area that is able to zoom and pan on the nested content.
 *
 * @brief The PannableScrollArea class
 */
class PannableScrollArea : public QScrollArea
{
    const double SCALE_STEP_SIZE = 0.1;

    double scale = 1.0;

public:
    PannableScrollArea(QWidget *parent = nullptr);

    void resizeWidget();

public slots:
    void fitWindow();
    void zoomIn();
    void zoomOut();

protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // PANNABLESCROLLAREA_H
