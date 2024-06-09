#include "drawing/model/volume_draw_properties.h"

/**
 * @brief VolumeDrawProperties::VolumeDrawProperties
 */
VolumeDrawProperties::VolumeDrawProperties():
    render_type(VolumeRenderingType::ACCUMULATE),
    sample_steps(100),
    color_0({ 0., 0., 1. }),
    color_1({ 1., 1., 1. }),
    color_2({ 1., 0., 0. })
{
    camera_view_transformation.setToIdentity();
}
