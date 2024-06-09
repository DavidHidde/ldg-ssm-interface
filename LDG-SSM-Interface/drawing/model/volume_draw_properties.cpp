#include "drawing/model/volume_draw_properties.h"

/**
 * @brief VolumeDrawProperties::VolumeDrawProperties
 */
VolumeDrawProperties::VolumeDrawProperties():
    render_type(VolumeRenderingType::ACCUMULATE),
    sample_steps(100)
{
    camera_view_transformation.setToIdentity();
}
