#ifndef PANNABLESCROLLAREA_H
#define PANNABLESCROLLAREA_H

#include <QScrollArea>

/**
 * A scroll area, cinlduing controls for scaling and panning.
 *
 * @brief The PannableScrollArea class
 */
class PannableScrollArea : public QScrollArea
{
    const double SCALE_STEP_SIZE = 0.1;

    double scale = 1.0;

    void resizeWidget();

public:
    PannableScrollArea();

public slots:
    void fitWindow();
    void zoomIn();
    void zoomOut();

protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // PANNABLESCROLLAREA_H
