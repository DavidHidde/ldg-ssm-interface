#include "pannable_scroll_area.h"

/**
 * @brief PannableScrollArea::PannableScrollArea
 * @param parent
 */
PannableScrollArea::PannableScrollArea(QWidget *parent):
    QScrollArea(parent)
{
    setAlignment(Qt::AlignCenter);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFrameShape(QFrame::NoFrame);
}

/**
 * @brief PannableScrollArea::PannableScrollArea Scale and square the inner widget.
 */
void PannableScrollArea::resizeWidget()
{
    if (widget() != nullptr) {
        int scaled_max_dim = std::round(scale * static_cast<double>(std::min(width(), height())));
        widget()->resize(scaled_max_dim, scaled_max_dim);
    }
}

/**
 * @brief PannableScrollArea::fitWindow Reset the scale and fit to the window.
 */
void PannableScrollArea::fitWindow()
{
    scale = 1.;
    resizeWidget();
}

/**
 * @brief PannableScrollArea::zoomIn
 */
void PannableScrollArea::zoomIn()
{
    scale += SCALE_STEP_SIZE;
    resizeWidget();
}

/**
 * @brief PannableScrollArea::zoomOut
 */
void PannableScrollArea::zoomOut()
{
    if (scale > SCALE_STEP_SIZE) {
        scale -= SCALE_STEP_SIZE;
        resizeWidget();
    }
}

/**
 * @brief PannableScrollArea::resizeEvent Resize the widget to square.
 * @param event
 */
void PannableScrollArea::resizeEvent(QResizeEvent *event)
{
    QScrollArea::resizeEvent(event);
    resizeWidget();
}
