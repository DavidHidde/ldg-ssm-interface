#ifndef IMAGE_ATLAS_H
#define IMAGE_ATLAS_H

#include <QImage>
#include <QVector3D>
#include <QMap>

#include <drawing/model/tree_draw_properties.h>

/**
 * @brief The AtlasContainer class Container containing either an image atlas or a volume atlas.
 */
struct AtlasContainer
{
    QMap<QPair<size_t, size_t>, QVector3D> mapping; // Mapping of the [height, index] to a vector of [u, v, w].
    QVector3D coord_offsets;                        // [u, v, w] offsets to apply to the mapping origin.
    QList<unsigned char> data;                      // Actual data of the atlas, to be loaded into an OpenGL Texture
    std::array<size_t, 3> dims;
};

AtlasContainer createImageAtlasContainer(TreeDrawProperties *draw_properties, size_t max_2D_texture_dim);
AtlasContainer createVolumeAtlasContainer(TreeDrawProperties *draw_properties, size_t max_3D_texture_dim);

#endif // IMAGE_ATLAS_H
