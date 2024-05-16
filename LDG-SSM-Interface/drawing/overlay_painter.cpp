#include "overlay_painter.h"

/**
 * @brief OverlayPainter::OverlayPainter
 * @param device
 * @param tree_properties
 * @param window_properties
 */
OverlayPainter::OverlayPainter(QPaintDevice *device, TreeDrawProperties *tree_properties, WindowDrawProperties *window_properties):
    tree_properties(tree_properties),
    window_properties(window_properties),
    QPainter(device)
{
}

/**
 * @brief OverlayPainter::drawOverlay Draw the grid overlay based on the tree dimensions and state.
 *
 * See https://doc.qt.io/qt-6/coordsys.html#aliased-painting and https://doc.qt.io/qt-6/qpainter.html#drawRect
 *
 */
void OverlayPainter::drawOverlay()
{
    // Set the pen
    QPen new_pen;
    new_pen.setWidth(1);
    if (tree_properties->background_color.x() + tree_properties->background_color.y() + tree_properties->background_color.z() < 1.5)
        new_pen.setBrush(Qt::white);
    else
        new_pen.setBrush(Qt::black);
    setPen(new_pen);

    // Draw the overlay
    double pen_width = new_pen.width();
    double before_pixels = pen_width / 2;

    for (auto &[height, index] : tree_properties->draw_array) {
        double side_len = window_properties->height_node_lens[height];
        auto [num_rows, num_cols] = tree_properties->height_dims[height];
        double x = index % num_cols;
        double y = index / num_cols;

        drawRect(
            std::round(x * (side_len + window_properties->node_spacing) + before_pixels),
            std::round(y * (side_len + window_properties->node_spacing) + before_pixels),
            side_len - pen_width,
            side_len - pen_width
        );
    }
}
