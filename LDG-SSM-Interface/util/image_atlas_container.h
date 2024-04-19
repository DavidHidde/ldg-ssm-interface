#ifndef IMAGE_ATLAS_H
#define IMAGE_ATLAS_H

#include <QImage>
#include <QVector3D>
#include <QMap>

/**
 * @brief The ImageAtlasContainer class Container containing image atlasses which are based on multiple individual images. Images are assumed to already be the same dimension.
 */
struct ImageAtlasContainer
{
    QMap<QPair<size_t, size_t>, QVector3D> mapping; // Mapping of the [height, index] to a vector of [u, v, atlas_idx].
    QVector2D coord_offsets;                        // [u, v] offsets to apply to the mapping origin.
    QList<QImage> image_atlasses;

    ImageAtlasContainer(QMap<QPair<size_t, size_t>, QVector3D> mapping, QVector2D coord_offsets, QList<QImage> image_atlasses);
};

ImageAtlasContainer createAtlasContainer(QMap<QPair<size_t, size_t>, QPair<QImage, double>> *data, size_t max_atlas_dim);

#endif // IMAGE_ATLAS_H
