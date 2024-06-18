#include "pannable_scroll_area.h"
#include <QScrollBar>

/**
 * @brief PannableScrollArea::PannableScrollArea
 * @param parent
 */
PannableScrollArea::PannableScrollArea(QWidget *parent):
    QScrollArea(parent),
    tree_properties(nullptr),
    window_properties(nullptr),
    screen_controller(nullptr)
{
    viewport()->setMouseTracking(true);
    setAlignment(Qt::AlignCenter);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFrameShape(QFrame::NoFrame);
}

/**
 * @brief PannableScrollArea::intialize
 * @param window_properties
 * @param tree_properties
 * @param screen_controller
 */
void PannableScrollArea::intialize(WindowDrawProperties *window_properties, TreeDrawProperties *tree_properties, ScreenController *screen_controller)
{
    this->window_properties = window_properties;
    this->tree_properties = tree_properties;
    this->screen_controller = screen_controller;
}

/**
 * @brief PannableScrollArea::updateViewport
 */
void PannableScrollArea::updateViewport()
{
    if (window_properties != nullptr) {
        int overflow_width = std::max(0, static_cast<int>(window_properties->scaled_window_size.x() - width()));
        int overflow_height = std::max(0, static_cast<int>(window_properties->scaled_window_size.y() - height()));

        horizontalScrollBar()->setMaximum(overflow_width);
        verticalScrollBar()->setMaximum(overflow_height);
        setHorizontalScrollBarPolicy(overflow_width > 0 ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(overflow_height > 0 ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff);

        window_properties->draw_origin = {
            std::max(0.f, (static_cast<float>(width()) - window_properties->scaled_window_size.x() + 2) / 2.f) - horizontalScrollBar()->value(),
            std::max(0.f, (static_cast<float>(height()) - window_properties->scaled_window_size.y() + 2) / 2.f) - verticalScrollBar()->value()
        };
    }
}

/**
 * @brief PannableScrollArea::updateWindowProperties Update the screen-space dimensions according to the current window.
 */
void PannableScrollArea::updateWindowProperties()
{
    if (window_properties != nullptr && tree_properties != nullptr) {
        window_properties->height_node_lens = QList<double>(tree_properties->tree_max_height + 1, 0.);
        double side_len = window_properties->scaled_window_size.x() - 2.; // Subtract 2 pixels to make sure we stay within the borders
        for (int curr_height = tree_properties->tree_max_height; curr_height >= 0; --curr_height) {
            window_properties->height_node_lens[curr_height] = side_len;
            side_len = (side_len - window_properties->node_spacing) / 2.;
        }

        tree_properties->gl_space_scale_vector = QVector3D{
            2.f / (static_cast<float>(width()) * window_properties->device_pixel_ratio),
            2.f / (static_cast<float>(height()) * window_properties->device_pixel_ratio),
            1.f
        };
    }
}

/**
 * @brief PannableScrollArea::isInitialized
 * @return
 */
bool PannableScrollArea::isInitialized()
{
    return widget() != nullptr && window_properties != nullptr && tree_properties != nullptr && screen_controller != nullptr;
}

/**
 * @brief PannableScrollArea::isReady
 * @return
 */
bool PannableScrollArea::isReady()
{
    return isInitialized() && !tree_properties->draw_array.isEmpty();
}

/**
 * @brief PannableScrollArea::fitWindow Reset the scale and fit to the window.
 */
void PannableScrollArea::fitWindow()
{
    if (isInitialized()) {
        window_properties->scale = 1.;
        window_properties->scaled_window_size = window_properties->scale * window_properties->base_window_size;

        updateViewport();
        updateWindowProperties();
        emit viewportSizeChanged();
    }
}

/**
 * @brief PannableScrollArea::zoomIn
 */
void PannableScrollArea::zoomIn()
{
    if (isInitialized()) {
        window_properties->scale *= 1 + SCALE_STEP_SIZE;
        window_properties->scaled_window_size = window_properties->scale * window_properties->base_window_size;

        updateViewport();
        updateWindowProperties();
        emit viewportSizeChanged();
    }
}

/**
 * @brief PannableScrollArea::zoomOut
 */
void PannableScrollArea::zoomOut()
{
    if (isInitialized() && window_properties->scale > SCALE_STEP_SIZE) {
        window_properties->scale /= 1 + SCALE_STEP_SIZE;
        window_properties->scaled_window_size = window_properties->scale * window_properties->base_window_size;

        updateViewport();
        updateWindowProperties();
        emit viewportSizeChanged();
    }
}

/**
 * @brief PannableScrollArea::scrollContentsBy
 * @param dx
 * @param dy
 */
void PannableScrollArea::scrollContentsBy(int dx, int dy)
{
    if (isInitialized()) {
        updateViewport();
        emit viewportPositionChanged();
    }
}

/**
 * @brief PannableScrollArea::resizeEvent Resize the widget to square.
 * @param event
 */
void PannableScrollArea::resizeEvent(QResizeEvent *event)
{
    QScrollArea::resizeEvent(event);

    if (isInitialized()) {
        float max_dim = std::min(width(), height());
        window_properties->base_window_size = { max_dim, max_dim };
        window_properties->scaled_window_size = window_properties->scale * window_properties->base_window_size;

        updateViewport();
        updateWindowProperties();
    }

    widget()->resize(width(), height());
}

/**
 * @brief PannableScrollArea::mousePressEvent
 * @param event
 */
void PannableScrollArea::mousePressEvent(QMouseEvent *event)
{
    if (isReady())
        screen_controller->handleMousePressEvent(event);
}

/**
 * @brief PannableScrollArea::mouseMoveEvent
 * @param event
 */
void PannableScrollArea::mouseMoveEvent(QMouseEvent *event)
{
    if (isReady())
        screen_controller->handleMouseMoveEvent(event, width(), height());
}

/**
 * @brief PannableScrollArea::wheelEvent
 * @param event
 */
void PannableScrollArea::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier && isReady())
        screen_controller->handleWheelEvent(event);
    else
        QScrollArea::wheelEvent(event);
}

/**
 * @brief PannableScrollArea::viewportEvent Override to properly capture mouse events.
 * @param event
 * @return
 */
bool PannableScrollArea::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::MouseMove) {
        mouseMoveEvent(static_cast<QMouseEvent*>(event));
        return true;
    }
    if (event->type() == QEvent::MouseButtonPress) {
        mousePressEvent(static_cast<QMouseEvent*>(event));
        return true;
    }
    return QScrollArea::viewportEvent(event);
}

/**
 * @brief PannableScrollArea::screenChanged Update the dimensions when changing screens
 */
void PannableScrollArea::screenChanged()
{
    if (isInitialized()) {
        window_properties->device_pixel_ratio = devicePixelRatio();
        updateWindowProperties();
        emit viewportSizeChanged();
    }
}
