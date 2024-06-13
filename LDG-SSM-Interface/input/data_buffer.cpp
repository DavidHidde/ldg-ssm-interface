#include "input/data_buffer.h"

#include "QtCore/qdebug.h"
#include "input/data.h"
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
 * @return True if the operation succeeded, else false
 */
bool readInput(QString visualization_configuration_path, TreeDrawProperties &tree_properties)
{
    // Load all config files
    VisualizationConfiguration config;
    if (!config.fromJSONFile(visualization_configuration_path))
        return false;

    QString config_dir_path = (QFileInfo(visualization_configuration_path)).path();
    InputConfiguration disparity_config;
    InputConfiguration vis_data_config;
    if (
        !disparity_config.fromJSONFile(fixPath(config.disparity_config_path, config_dir_path)) ||
        !vis_data_config.fromJSONFile(fixPath(config.visualization_config_path, config_dir_path))
    )
        return false;

    if (vis_data_config.grid_dims != disparity_config.grid_dims) {
        qDebug() << "Grid dims of configs are not equal!";
        return false;
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
    if (readFileIntoBuffer(assignment_buffer, data_path) != idx) {
        qDebug() << "Sizes:" << assignment_buffer.size() << idx;
        qDebug() << "Unable to load data from file \"" << data_path << "\"\n";
        return false;
    }

    // Load visualization data
    size_t data_elem_size = vis_data_config.data_dims[0] * vis_data_config.data_dims[1] * vis_data_config.data_dims[2];
    std::vector<unsigned char> data_buffer(vis_data_config.num_elements * data_elem_size);
    data_path = fixPath(vis_data_config.data_path, (QFileInfo(fixPath(config.visualization_config_path, config_dir_path))).path());
    if (readFileIntoBuffer(data_buffer, data_path) != vis_data_config.num_elements * data_elem_size) {
        qDebug() << "Sizes:" << data_buffer.size() << vis_data_config.num_elements * data_elem_size;
        qDebug() << "Unable to load data from file \"" << data_path << "\"\n";
        return false;
    }

    // Load disparities
    std::vector<double> disparity_buffer(idx, -1);
    data_path = fixPath(disparity_config.data_path, (QFileInfo(fixPath(config.disparity_config_path, config_dir_path))).path());
    if (readFileIntoBuffer(disparity_buffer, data_path) != disparity_config.num_elements) {
        qDebug() << "Sizes:" << disparity_buffer.size() << disparity_config.num_elements;
        qDebug() << "Unable to load data from file \"" << data_path << "\"\n";
        return false;
    }

    // Combine everything into a single data map
    QMap<QPair<size_t, size_t>, QList<unsigned char>> *data_map = new QMap<QPair<size_t, size_t>, QList<unsigned char>>;
    QMap<QPair<size_t, size_t>, double> disparity_map;
    QSet<QPair<size_t, size_t>> invalid_nodes;
    for (size_t height = 0; height < max_height; ++height) {
        auto [start, end] = start_ends[height];
        for (size_t idx = 0; start + idx < end;  ++idx) {
            int assigned_idx = assignment_buffer[start + idx];
            if (assigned_idx >= 0) {
                unsigned char *start_ptr = data_buffer.data() + data_elem_size * assigned_idx;
                disparity_map[{ height, idx }] = disparity_buffer[assigned_idx];
                (*data_map)[{ height, idx }] = QList<unsigned char>(start_ptr, start_ptr + data_elem_size);
            } else {
                invalid_nodes.insert({ height, idx });
            }
        }
    }

    // Set loaded properties. Some other properties will be set dynamically later as they depend on the screen size.
    tree_properties.tree_max_height = max_height - 1;
    tree_properties.height_dims = height_dims;
    tree_properties.draw_type = vis_data_config.data_dims[2] > 4 ? DrawType::VOLUME : DrawType::IMAGE;
    tree_properties.draw_array = { { max_height - 1, 0 } };
    tree_properties.invalid_nodes = invalid_nodes;
    tree_properties.data = data_map;
    tree_properties.disparities = disparity_map;
    tree_properties.data_dims = vis_data_config.data_dims;

    return true;
}
