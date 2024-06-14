#include "pannable_scroll_area.h"
#include <QScrollBar>

/**
 * @brief PannableScrollArea::PannableScrollArea
 * @param parent
 */
PannableScrollArea::PannableScrollArea(QWidget *parent):
    QScrollArea(parent)
{
    setMouseTracking(true);
    setAlignment(Qt::AlignCenter);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFrameShape(QFrame::NoFrame);
}

/**
 * @brief PannableScrollArea::intialize
 * @param window_properties
 * @param tree_properties
 * @param grid_controller
 */
void PannableScrollArea::intialize(WindowDrawProperties *window_properties, TreeDrawProperties *tree_properties, GridController *grid_controller)
{
    this->window_properties = window_properties;
    this->tree_properties = tree_properties;
    this->grid_controller = grid_controller;
}

/**
 * @brief PannableScrollArea::updateViewport
 */
void PannableScrollArea::updateViewport()
{
    if (window_properties != nullptr) {
        int scaled_max_dim = std::round(window_properties->scale * static_cast<double>(std::min(width(), height())));
        window_properties->current_viewport = QRect{
            horizontalScrollBar()->value(),
            verticalScrollBar()->value(),
            std::min(width(), scaled_max_dim),
            std::min(height(), scaled_max_dim)
        };
    }
}

/**
 * @brief PannableScrollArea::updateWindowProperties Update the window properties according to the current window size.
 */
void PannableScrollArea::updateWindowProperties()
{
    if (window_properties != nullptr && tree_properties != nullptr) {
        float scaled_max_dim = std::round(window_properties->scale * static_cast<double>(std::min(width(), height())));

        window_properties->height_node_lens = QList<double>(tree_properties->tree_max_height + 1, 0.);
        window_properties->device_pixel_ratio = devicePixelRatio();
        window_properties->window_size = { scaled_max_dim, scaled_max_dim };

        double side_len = scaled_max_dim - 2.; // Subtract 2 pixels to make sure we stay within the borders
        for (int curr_height = tree_properties->tree_max_height; curr_height >= 0; --curr_height) {
            window_properties->height_node_lens[curr_height] = side_len;
            side_len = (side_len - window_properties->node_spacing) / 2.;
        }

        float opengl_dim = scaled_max_dim * window_properties->device_pixel_ratio;
        tree_properties->gl_space_scale_vector = QVector3D{
            2.f / opengl_dim,
            2.f / opengl_dim,
            1.f
        };
    }
}

/**
 * @brief PannableScrollArea::isReady
 * @return
 */
bool PannableScrollArea::isReady()
{
    return widget() != nullptr && window_properties != nullptr && tree_properties != nullptr && grid_controller != nullptr && !tree_properties->draw_array.isEmpty();
}

/**
 * @brief PannableScrollArea::PannableScrollArea Scale and square the inner widget.
 */
void PannableScrollArea::resizeWidget()
{
    int scaled_max_dim = std::round(window_properties->scale * static_cast<double>(std::min(width(), height())));
    updateViewport();
    updateWindowProperties();
    widget()->resize(scaled_max_dim, scaled_max_dim);
}

/**
 * @brief PannableScrollArea::fitWindow Reset the scale and fit to the window.
 */
void PannableScrollArea::fitWindow()
{
    if (isReady()) {
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
    if (isReady()) {
        window_properties->scale *= 1 + SCALE_STEP_SIZE;
        updateViewport();
        resizeWidget();
    }
}

/**
 * @brief PannableScrollArea::zoomOut
 */
void PannableScrollArea::zoomOut()
{
    if (isReady() && window_properties->scale > SCALE_STEP_SIZE) {
        window_properties->scale /= 1 + SCALE_STEP_SIZE;
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
    if (isReady()) {
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

/**
 * @brief PannableScrollArea::mousePressEvent
 * @param event
 */
void PannableScrollArea::mousePressEvent(QMouseEvent *event)
{
    if (isReady())
        grid_controller->handleMouseClick(event);
}

/**
 * @brief PannableScrollArea::mouseMoveEvent
 * @param event
 */
void PannableScrollArea::mouseMoveEvent(QMouseEvent *event)
{
    if (isReady())
        grid_controller->handleMouseMoveEvent(event);
}

/**
 * @brief PannableScrollArea::wheelEvent
 * @param event
 */
void PannableScrollArea::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier && isReady())
        grid_controller->handleMouseScrollEvent(event);
    else
        QScrollArea::wheelEvent(event);
}

/**
 * @brief PannableScrollArea::screenChanged Update the dimensions when changing screens
 */
void PannableScrollArea::screenChanged()
{
    updateWindowProperties();
    widget()->update();
}
