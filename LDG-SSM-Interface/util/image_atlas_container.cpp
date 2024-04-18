#include "image_atlas_container.h"
#include <QPainter>
#include <QDebug>

/**
 * @brief ImageAtlasContainer::ImageAtlasContainer
 * @param mapping
 * @param coord_offsets
 * @param image_atlasses
 */
ImageAtlasContainer::ImageAtlasContainer(QMap<QPair<size_t, size_t>, QVector3D> mapping, QVector2D coord_offsets, QList<QImage> image_atlasses):
    mapping(mapping),
    coord_offsets(coord_offsets),
    image_atlasses(image_atlasses)
{
}

/**
 * @brief createAtlasContainer Create image atlassses for the given data set.
 *  Images are assumed to share one dimension, which should be the largest dimension. Resulting atlasses will be square with centered images.
 * @param data
 * @param max_atlas_dim
 * @return
 */
ImageAtlasContainer createAtlasContainer(QMap<QPair<size_t, size_t>, QPair<QImage, double>> *data, size_t max_atlas_dim)
{
    auto [first_img, first_disparity] = data->first();
    float image_dim = std::max(first_img.width(), first_img.height());
    size_t images_per_atlas_dim = std::floor(static_cast<float>(max_atlas_dim) / image_dim);
    size_t images_per_atlas = images_per_atlas_dim * images_per_atlas_dim;
    size_t num_atlasses = std::ceil(static_cast<float>(data->size()) / static_cast<float>(images_per_atlas));
    int atlas_dim = images_per_atlas_dim * image_dim;

    // Initialize container contents
    QList<QImage> image_atlasses(num_atlasses, QImage{ atlas_dim, atlas_dim, QImage::Format_RGB32 });
    for (auto &img : image_atlasses)
        img.fill(Qt::black);
    QVector2D coord_offsets = QVector2D{image_dim, image_dim} / QVector2D{ static_cast<float>(atlas_dim), static_cast<float>(atlas_dim) };
    QMap<QPair<size_t, size_t>, QVector3D> mapping;

    // Fill mapping and atlasses
    size_t count = 0;
    for (auto [key, data] : data->asKeyValueRange()) {
        auto [height, index] = key;
        auto [image, disparity] = data;
        int atlas_idx = count / images_per_atlas;
        int canvas_idx = count % images_per_atlas;
        int canvas_x = (canvas_idx % images_per_atlas_dim) * image_dim;
        int canvas_y = (canvas_idx / images_per_atlas_dim) * image_dim;
        QPainter painter(&image_atlasses[atlas_idx]);
        painter.drawImage(
            canvas_x + (image_dim - image.width()) / 2, // Center x
            canvas_y + (image_dim - image.height()) / 2, // Center y
            image
        );
        mapping[{ height, index }] = QVector3D{
            static_cast<float>(canvas_x) / static_cast<float>(atlas_dim),
            static_cast<float>(canvas_y) / static_cast<float>(atlas_dim),
            static_cast<float>(atlas_idx)
        };

        ++count;
    }

    return { mapping, coord_offsets, image_atlasses };
}
