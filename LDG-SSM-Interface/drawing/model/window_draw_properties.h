#ifndef WINDOW_DRAW_PROPERTIES_H
#define WINDOW_DRAW_PROPERTIES_H

#include <QList>
#include <QVector2D>

/**
 * @brief The WindowDrawProperties class Properties concerning pixel spacing and sizes.
 */
struct WindowDrawProperties
{
    // Calculated properties
    double node_spacing;                // Spacing between nodes in the grid.
    QList<double> height_node_lens;     // Lengths of the nodes at each height.

    // Dynamic window specific properties
    double scale;
    float device_pixel_ratio;

    QVector2D draw_origin;              // Start pixel coordinates for drawing.
    QVector2D base_window_size;         // The size of the window at a scaling of 1.
    QVector2D scaled_window_size;       // Base window size scaled by scale.
};

#endif // WINDOW_DRAW_PROPERTIES_H
