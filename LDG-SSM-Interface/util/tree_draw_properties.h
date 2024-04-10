#ifndef TREEDRAWPROPERTIES_H
#define TREEDRAWPROPERTIES_H

#include <cstddef>
#include <QPoint>
#include <QList>

/**
 * @brief The TreeDrawProperties class Properties needed to draw the quad tree in a simple POD container.
 */
struct TreeDrawProperties
{
    // Draw properties
    size_t node_spacing;                            // Spacing between nodes in the grid.
    QPoint grid_origin;                             // Origin point of the grid.
    QList<std::pair<size_t, size_t>> draw_array;    // The elements that should be drawn, consisting of [height, index] pairs.
    QList<size_t> click_assignment;                 // Array of the base grid size pointing to indices within the draw array resembling the clicked elements.
    QList<size_t> height_node_lens;                 // Lengths of the nodes at each height.

    // Tree dimensions
    const size_t tree_max_height;                         // Max height of the quad tree.
    const QList<std::pair<size_t, size_t>> height_dims;   // Dimensions per height of the tree.

};

#endif // TREEDRAWPROPERTIES_H
