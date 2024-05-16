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
    QVector2D window_size;              // Total window size in width x height
    QRect current_viewport;             // The currently visible viewport
};

#endif // WINDOW_DRAW_PROPERTIES_H
