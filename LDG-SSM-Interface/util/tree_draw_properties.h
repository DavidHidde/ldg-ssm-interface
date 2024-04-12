#ifndef TREEDRAWPROPERTIES_H
#define TREEDRAWPROPERTIES_H

#include <cstddef>
#include <QList>
#include <QMatrix4x4>

/**
 * @brief The TreeDrawProperties class Properties needed to draw the quad tree in a simple POD container.
 */
struct TreeDrawProperties
{
    // Tree dimensions
    const size_t tree_max_height;
    const QList<std::pair<size_t, size_t>> height_dims;

    // Data
    QSet<std::pair<size_t, size_t>> draw_array;             // The elements that should be drawn, consisting of [height, index] pairs.

    // Screen space - Pixel sizes
    double node_spacing;                                    // Spacing between nodes in the grid.
    QList<double> height_node_lens;                         // Lengths of the nodes at each height.

    // OpenGL space - 3D projection matrices
    QVector3D gl_space_scale_vector;                        // Scaling fcator for scaling from sceen space to OpenGL world space.
    QMatrix4x4 model_view;
    QMatrix4x4 projection;
    QRect viewport;
};

#endif // TREEDRAWPROPERTIES_H
