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

    // Isosurface specific
    float threshold;

    VolumeDrawProperties();
};

#endif // VOLUME_DRAW_PROPERTIES_H
