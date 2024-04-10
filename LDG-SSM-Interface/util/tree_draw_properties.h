#ifndef TREEDRAWPROPERTIES_H
#define TREEDRAWPROPERTIES_H

#include <cstddef>
#include <QList>

/**
 * @brief The TreeDrawProperties class Properties needed to draw the quad tree in a simple POD container.
 */
struct TreeDrawProperties
{
    // Draw properties
    size_t node_spacing;                            // Spacing between nodes in the grid (screen space).
    double gl_space_scale_factor;                   // Scaling fcator for scaling from sceen space to OpenGL world space.
    QList<std::pair<size_t, size_t>> draw_array;    // The elements that should be drawn, consisting of [height, index] pairs.
    QList<int> click_assignment;                    // Square row-major array (nearest power of 2) to indices within the draw array resembling the clicked elements. -1 indicates a miss.
    QList<size_t> height_node_lens;                 // Lengths of the nodes at each height (screen space).

    // Tree dimensions
    const size_t tree_max_height;                         // Max height of the quad tree.
    const QList<std::pair<size_t, size_t>> height_dims;   // Dimensions per height of the tree.

};

#endif // TREEDRAWPROPERTIES_H
