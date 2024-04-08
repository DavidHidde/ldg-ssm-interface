#include "pannable_scroll_area.h"

/**
 * @brief PannableScrollArea::PannableScrollArea
 */
void PannableScrollArea::resizeWidget()
{
    auto max_dim = height() > width() ? scale * width() : scale * height();
    if (widget() != nullptr)
        widget()->resize(max_dim, max_dim);
}

PannableScrollArea::PannableScrollArea()
{
    setAlignment(Qt::AlignCenter);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
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
    if (scale > SCALE_STEP_SIZE)
        scale -= SCALE_STEP_SIZE;

    resizeWidget();
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
