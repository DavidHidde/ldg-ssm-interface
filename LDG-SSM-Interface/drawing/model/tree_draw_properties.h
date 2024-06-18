#ifndef TREEDRAWPROPERTIES_H
#define TREEDRAWPROPERTIES_H

#include "drawing/model/types.h"
#include <cstddef>
#include <QList>
#include <QMatrix4x4>

/**
 * @brief The TreeDrawProperties class Properties needed to draw the quad tree in a simple POD container.
 */
struct TreeDrawProperties
{
    // Tree dimensions
    size_t tree_max_height;
    QList<std::pair<size_t, size_t>> height_dims;

    // General draw info
    DrawType draw_type;
    QSet<std::pair<size_t, size_t>> draw_array;                 // The elements that should be drawn, consisting of [height, index] pairs.
    QSet<std::pair<size_t, size_t>> invalid_nodes;              // Void tile nodes.

    // Base data
    QMap<QPair<size_t, size_t>, QList<unsigned char>> *data;    // The actual data, indexed by [height, index] pairs. The data consists of the raw data, which should be managed externally.
    QMap<QPair<size_t, size_t>, double> disparities;            // Disparity value per node, indexed by [height, index] pairs.
    std::array<size_t, 3> data_dims;

    // OpenGL space - 3D projection
    QVector3D gl_space_scale_vector;                            // Scaling factor for scaling from sceen space to OpenGL world space.
    QMatrix4x4 projection;
    QVector3D background_color;

    TreeDrawProperties();
};

#endif // TREEDRAWPROPERTIES_H
