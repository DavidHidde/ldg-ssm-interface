#ifndef VOLUME_DRAW_PROPERTIES_H
#define VOLUME_DRAW_PROPERTIES_H

#include "drawing/model/types.h"

#include <QMatrix4x4>

/**
 * @brief The VolumeDrawProperties class Properties specifically aimed at drawing volumes in a simple POD container.
 */
struct VolumeDrawProperties
{
    VolumeRenderingType render_type;
    QMatrix4x4 camera_view_transformation;

    size_t sample_steps;
    QVector3D color_0;
    QVector3D color_1;
    QVector3D color_2;

    // Isosurface specific
    float threshold;
};

#endif // VOLUME_DRAW_PROPERTIES_H
