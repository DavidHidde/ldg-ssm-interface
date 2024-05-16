#include "pannable_scroll_area.h"
#include <QScrollBar>

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
 * @brief PannableScrollArea::setWindowDrawProperties
 * @param window_properties
 */
void PannableScrollArea::setWindowDrawProperties(WindowDrawProperties *window_properties)
{
    this->window_properties = window_properties;
}

/**
 * @brief PannableScrollArea::PannableScrollArea Scale and square the inner widget.
 */
void PannableScrollArea::resizeWidget()
{
    if (widget() != nullptr) {
        int scaled_max_dim = std::round(window_properties->scale * static_cast<double>(std::min(width(), height())));
        updateViewport();
        widget()->resize(scaled_max_dim, scaled_max_dim);
    }
}

/**
 * @brief PannableScrollArea::updateViewport
 */
void PannableScrollArea::updateViewport()
{
    int scaled_max_dim = std::round(window_properties->scale * static_cast<double>(std::min(width(), height())));
    window_properties->current_viewport = QRect{
        horizontalScrollBar()->value(),
        verticalScrollBar()->value(),
        std::min(width(), scaled_max_dim),
        std::min(height(), scaled_max_dim)
    };
}

/**
 * @brief PannableScrollArea::fitWindow Reset the scale and fit to the window.
 */
void PannableScrollArea::fitWindow()
{
    if (widget() != nullptr) {
        window_properties->scale = 1.;
        updateViewport();
        resizeWidget();
    }
}

/**
 * @brief PannableScrollArea::zoomIn
 */
void PannableScrollArea::zoomIn()
{
    if (widget() != nullptr) {
        window_properties->scale += SCALE_STEP_SIZE;
        updateViewport();
        resizeWidget();
    }
}

/**
 * @brief PannableScrollArea::zoomOut
 */
void PannableScrollArea::zoomOut()
{
    if (widget() != nullptr && window_properties->scale > SCALE_STEP_SIZE) {
        window_properties->scale -= SCALE_STEP_SIZE;
        updateViewport();
        resizeWidget();
    }
}

/**
 * @brief PannableScrollArea::scrollContentsBy
 * @param dx
 * @param dy
 */
void PannableScrollArea::scrollContentsBy(int dx, int dy)
{
    if (widget() != nullptr) {
        updateViewport();
        widget()->update();
    }
    QScrollArea::scrollContentsBy(dx, dy);
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
