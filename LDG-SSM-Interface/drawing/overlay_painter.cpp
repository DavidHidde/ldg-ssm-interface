#include "overlay_painter.h"

/**
 * @brief OverlayPainter::OverlayPainter
 * @param device
 * @param draw_properties
 */
OverlayPainter::OverlayPainter(QPaintDevice *device, TreeDrawProperties *draw_properties):
    draw_properties(draw_properties),
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
    new_pen.setBrush(Qt::white);
    setPen(new_pen);

    // Draw the overlay
    double pen_width = new_pen.width();
    double before_pixels = pen_width / 2;

    for (auto &[height, index] : draw_properties->draw_array) {
        double side_len = draw_properties->height_node_lens[height];
        auto [num_rows, num_cols] = draw_properties->height_dims[height];
        double x = index % num_cols;
        double y = index / num_cols;

        drawRect(
            std::round(x * (side_len + draw_properties->node_spacing) + before_pixels),
            std::round(y * (side_len + draw_properties->node_spacing) + before_pixels),
            side_len - pen_width,
            side_len - pen_width
        );
    }
}
