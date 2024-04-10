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
 */
void OverlayPainter::drawOverlay()
{
    auto pen_width = pen().width();

    for (auto [height, index] : draw_properties->draw_array) {
        auto side_len = draw_properties->height_node_lens[height];
        auto [num_rows, num_cols] = draw_properties->height_dims[height];
        int x = index % num_cols;
        int y = index / num_cols;

        // Rects with a border have a size of rect.size() + pen_width. See https://doc.qt.io/qt-6/qpainter.html#drawRect
        // This means the origin is (pen_width, pen_width) and all sides need to be side_len - pen_width long.
        drawRect(
            x * (side_len - pen_width + draw_properties->node_spacing) + std::max(0, x - 1) * draw_properties->node_spacing + pen_width,
            y * (side_len - pen_width + draw_properties->node_spacing) + std::max(0, y - 1) * draw_properties->node_spacing + pen_width,
            side_len - 2 * pen_width,
            side_len - 2 * pen_width
        );
    }
}
