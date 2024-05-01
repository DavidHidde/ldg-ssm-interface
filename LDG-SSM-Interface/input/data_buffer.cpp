#include "input/data_buffer.h"

#include "helper_bzip.h"
#include "QtCore/qdebug.h"
#include "input_configuration.h"
#include "visualization_configuration.h"

#include <QFileInfo>

/**
 * Add the config dir path to paths that are relative from the config.
 *
 * @brief fixPath
 * @param file_path
 * @param config_dir
 * @return
 */
QString fixPath(QString file_path, QString config_dir)
{
    return(QFileInfo(file_path)).isRelative() ? config_dir + "/" + file_path : file_path;
}

/**
 * @brief readInput
 * @param visualization_configuration_path
 * @return A map using the [height, index] of a cell as keys for the [image, disparity] values
 */
QPair<QMap<QPair<size_t, size_t>, QPair<QImage, double>> *, TreeDrawProperties *> readInput(QString visualization_configuration_path)
{
    // Load all config files
    VisualizationConfiguration config;
    if (!config.fromJSONFile(visualization_configuration_path))
        return { nullptr, nullptr };

    QString config_dir_path = (QFileInfo(visualization_configuration_path)).path();
    InputConfiguration disparity_config;
    InputConfiguration vis_data_config;
    if (
        !disparity_config.fromJSONFile(fixPath(config.disparity_config_path, config_dir_path)) ||
        !vis_data_config.fromJSONFile(fixPath(config.visualization_config_path, config_dir_path))
    )
        return { nullptr, nullptr };

    if (vis_data_config.grid_dims != disparity_config.grid_dims) {
        qDebug() << "Grid dims of configs are not equal!";
        return { nullptr, nullptr };
    }


    // Load the assignment
    size_t max_height = 1;
    auto [num_rows, num_cols] = disparity_config.grid_dims;
    size_t idx = num_rows * num_cols;
    QList<QPair<size_t, size_t>> start_ends{ { 0, idx } };
    QList<QPair<size_t, size_t>> height_dims{ { num_rows, num_cols } };
    while (num_rows != 1 || num_cols != 1) {      
        num_rows = num_rows / 2 + num_rows % 2;
        num_cols = num_cols / 2 + num_cols % 2;
        start_ends.append({ idx, idx + num_rows * num_cols });
        height_dims.append({ num_rows, num_cols });
        idx += num_rows * num_cols;
        ++max_height;
    }

    std::vector<int> assignment_buffer(idx, -1);
    QString data_path = fixPath(config.assignment_path, config_dir_path);
    if (!helper::bzip_decompress(assignment_buffer, data_path.toStdString())) {
        qDebug() << "Unable to load data from file \"" << data_path << "\"\n";
        return { nullptr, nullptr };
    }

    // Load visualization data
    size_t data_elem_size = vis_data_config.data_dims[0] * vis_data_config.data_dims[1] * vis_data_config.data_dims[2];
    std::vector<unsigned char> *data_buffer = new std::vector<unsigned char>(vis_data_config.num_elements * data_elem_size);
    data_path = fixPath(vis_data_config.data_path, (QFileInfo(fixPath(config.visualization_config_path, config_dir_path))).path());
    if (!helper::bzip_decompress(*data_buffer, data_path.toStdString())) {
        qDebug() << "Unable to load data from file \"" << data_path << "\"\n";
        delete data_buffer;
        return { nullptr, nullptr };
    }

    // Load disparities
    std::vector<int> disparity_buffer(idx, -1);
    data_path = fixPath(disparity_config.data_path, (QFileInfo(fixPath(config.disparity_config_path, config_dir_path))).path());
    if (!helper::bzip_decompress(disparity_buffer, data_path.toStdString())) {
        qDebug() << "Unable to load data from file \"" << data_path << "\"\n";
        return { nullptr, nullptr };
    }

    // Combine everything into a single map
    QMap<QPair<size_t, size_t>, QPair<QImage, double>> *map = new QMap<QPair<size_t, size_t>, QPair<QImage, double>>;
    QSet<QPair<size_t, size_t>> invalid_nodes;
    for (size_t height = 0; height < max_height; ++height) {
        auto [start, end] = start_ends[height];
        for (size_t idx = 0; start + idx < end;  ++idx) {
            int assigned_idx = assignment_buffer[start + idx];
            if (assigned_idx >= 0) {
                (*map)[{ height, idx }] = {
                    QImage(
                        data_buffer->data() + data_elem_size * assigned_idx,
                        vis_data_config.data_dims[0],
                        vis_data_config.data_dims[1],
                        QImage::Format_RGBA8888,
                        [](void *data) { delete static_cast<uchar*>(data); }
                    ),
                    disparity_buffer[assigned_idx]
                };
            } else {
                invalid_nodes.insert({ height, idx });
            }
        }
    }

    // Set up properties
    TreeDrawProperties *properties = new TreeDrawProperties{ max_height - 1, height_dims };
    properties->draw_type = DrawType::VOLUME;
    properties->draw_array.insert({ max_height - 1, 0 }); // Contains root by default.
    properties->invalid_nodes = invalid_nodes;
    properties->node_spacing = 10.;  // Default spacing of 10.
    properties->height_node_lens = QList<double>(max_height, 0.);
    properties->background_color = { 1., 1., 1. };

    return { map, properties };
}
