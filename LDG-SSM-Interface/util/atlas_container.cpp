#include "atlas_container.h"
#include <QPainter>
#include <QDebug>

struct Color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

/**
 * @brief determineAtlasDims Determine the size of the atlas to be generated. For every image/volume, we allocate a square of max_dim dims.
 * The layout priority is x -> y -> z, so if the data is too large it will overflow in the z-direction.
 * @param draw_properties
 * @param max_texture_dim
 * @return [x, y, z] dims of the atlas along with the block size.
 */
QPair<std::array<size_t, 3>, size_t> determineAtlasDims(TreeDrawProperties *draw_properties, double max_texture_dim)
{
    auto [x_dim, y_dim, z_dim] = draw_properties->data_dims;
    double max_dim = std::max(std::max(x_dim, y_dim), std::max(x_dim, z_dim));
    double num_elements = draw_properties->data->size();

    double elements_per_dim = std::floor(max_texture_dim / max_dim);
    double num_rows = std::ceil(num_elements / elements_per_dim);
    double depth = std::ceil(num_rows / elements_per_dim);
    return {
        {
            static_cast<size_t>(std::min(num_elements, elements_per_dim) * max_dim),
            static_cast<size_t>(std::min(num_rows, elements_per_dim) * max_dim),
            static_cast<size_t>(draw_properties->draw_type == DrawType::IMAGE ? depth : depth * max_dim)
        },
        static_cast<size_t>(max_dim)
    };
}

/**
 * @brief createAtlasContainer Create an atlas container for images given the current data. Frees the tree data after the atlas is created.
 * @param draw_properties
 * @param max_2D_texture_dim
 * @return
 */
AtlasContainer createImageAtlasContainer(TreeDrawProperties *draw_properties, size_t max_2D_texture_dim)
{
    auto [atlas_dims, atlas_block_size] = determineAtlasDims(draw_properties, max_2D_texture_dim);
    auto [img_width, img_height, _] = draw_properties->data_dims;

    // Initialize container
    AtlasContainer container;
    container.dims = atlas_dims;
    container.coord_offsets = QVector3D{
        static_cast<float>(atlas_block_size) / static_cast<float>(atlas_dims[0]),
        static_cast<float>(atlas_block_size) / static_cast<float>(atlas_dims[1]),
        1.f
    };

    // Initialize atlas canvasses
    QList<QImage> image_atlasses(atlas_dims[2], QImage{ static_cast<int>(atlas_dims[0]), static_cast<int>(atlas_dims[1]), QImage::Format_RGB32 });
    for (auto &img : image_atlasses)
        img.fill(QColor{
            static_cast<int>(draw_properties->background_color.x() * 255),
            static_cast<int>(draw_properties->background_color.y() * 255),
            static_cast<int>(draw_properties->background_color.z() * 255),
        });

    int images_per_atlas = (atlas_dims[0] * atlas_dims[1]) / (atlas_block_size * atlas_block_size);
    int images_per_atlas_dim = std::floor(atlas_dims[0] / atlas_block_size);
    int x_img_offset = (atlas_block_size - img_width) / 2;
    int y_img_offset = (atlas_block_size - img_height) / 2;

    // Fill mapping and atlasses
    size_t count = 0;
    for (auto [key, data] : draw_properties->data->asKeyValueRange()) {
        auto &[height, index] = key;
        auto &[raw_image, _] = data;
        QImage image{ raw_image.data(), static_cast<int>(img_width), static_cast<int>(img_height), QImage::Format_RGBA8888 };
        int atlas_idx = count / images_per_atlas;
        int canvas_idx = count % images_per_atlas;
        int canvas_x = (canvas_idx % images_per_atlas_dim) * atlas_block_size;
        int canvas_y = (canvas_idx / images_per_atlas_dim) * atlas_block_size;
        QPainter painter(&image_atlasses[atlas_idx]);
        painter.drawImage(
            canvas_x + x_img_offset, // Center x
            canvas_y + y_img_offset, // Center y
            image
        );
        container.mapping[{ height, index }] = QVector3D{
            static_cast<float>(canvas_x) / static_cast<float>(atlas_dims[0]),
            static_cast<float>(canvas_y) / static_cast<float>(atlas_dims[1]),
            static_cast<float>(atlas_idx)
        };

        ++count;
    }

    // Copy atlas data to container data
    size_t data_offset = image_atlasses.first().bytesPerLine() * image_atlasses.first().height();
    for (size_t idx = 0; idx < image_atlasses.size(); ++idx) {
        auto bits = image_atlasses[idx].bits();
        container.data.append(QList(bits, bits + data_offset));
    }

    // Not needed anymore
    delete draw_properties->data;

    return container;
}

/**
 * @brief createVolumeAtlasContainer Create an atlas container for volumes given the current data. Frees the tree data after the atlas is created.
 * @param draw_properties
 * @param max_3D_texture_dim
 * @return
 */
AtlasContainer createVolumeAtlasContainer(TreeDrawProperties *draw_properties, size_t max_3D_texture_dim)
{
    auto [atlas_dims, atlas_block_size] = determineAtlasDims(draw_properties, max_3D_texture_dim);
    auto [volume_width, volume_height, volume_depth] = draw_properties->data_dims;

    // Initialize container
    AtlasContainer container;
    container.dims = atlas_dims;
    container.coord_offsets = QVector3D{
        static_cast<float>(atlas_block_size) / static_cast<float>(atlas_dims[0]),
        static_cast<float>(atlas_block_size) / static_cast<float>(atlas_dims[1]),
        static_cast<float>(atlas_block_size) / static_cast<float>(atlas_dims[2])
    };
    container.data = QList<unsigned char>(atlas_dims[0] * atlas_dims[1] * atlas_dims[2], 0);

    size_t volumes_per_atlas_dim = std::floor(atlas_dims[0] / atlas_block_size);
    size_t volumes_per_atlas_slice = volumes_per_atlas_dim * volumes_per_atlas_dim;

    size_t x_volume_offset = (atlas_block_size - volume_width) / 2;
    size_t y_volume_offset = (atlas_block_size - volume_height) / 2;
    size_t z_volume_offset = (atlas_block_size - volume_depth) / 2;

    size_t row_offset = atlas_dims[0];
    size_t slice_offset = atlas_dims[0] * atlas_dims[1];

    // Build the atlas by copying data from the volume buffers to the container buffer.
    size_t count = 0;
    for (auto [key, data] : draw_properties->data->asKeyValueRange()) {
        auto &[volume_data, _] = data;

        size_t atlas_x = count % volumes_per_atlas_dim;
        size_t atlas_y = (count % volumes_per_atlas_slice) / volumes_per_atlas_dim;
        size_t atlas_z = count / volumes_per_atlas_slice;
        size_t origin = atlas_x * atlas_block_size + x_volume_offset +
                        atlas_y * row_offset * atlas_block_size + y_volume_offset * row_offset +
                        atlas_z * slice_offset * atlas_block_size + z_volume_offset * slice_offset;

        container.mapping[key] = QVector3D{
            static_cast<float>(atlas_x * atlas_block_size) / static_cast<float>(atlas_dims[0]),
            static_cast<float>(atlas_y * atlas_block_size) / static_cast<float>(atlas_dims[1]),
            static_cast<float>(atlas_z * atlas_block_size) / static_cast<float>(atlas_dims[2])
        };

        for (size_t volume_z = 0; volume_z < volume_depth; ++volume_z) {
            for (size_t volume_y = 0; volume_y < volume_height; ++volume_y) {
                for (size_t volume_x = 0; volume_x < volume_width; ++volume_x) {
                    size_t container_idx = origin + volume_x + volume_y * row_offset + volume_z * slice_offset;
                    size_t volume_idx = volume_x + volume_y * volume_width + volume_z * volume_width * volume_height;
                    container.data[container_idx] = volume_data[volume_idx];   
                }
            }
        }

        ++count;
    }

    // Not needed anymore
    delete draw_properties->data;

    return container;
}
