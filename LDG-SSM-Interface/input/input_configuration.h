#ifndef INPUT_CONFIGURATION_H
#define INPUT_CONFIGURATION_H

#include <QString>
#include <array>
#include <cstddef>

/**
 * Types of input. Data should be loaded as doubles while visualizations should be loaded as unsigned chars.
 */
enum InputType
{
    DATA,
    VISUALIZATION
};

/**
 * Configuration of input data, which can be loaded from a JSON file.
 */
struct InputConfiguration
{
    InputType type;
    size_t num_elements;
    QString data_path;
    QPair<size_t, size_t> grid_dims;
    std::array<size_t, 3> data_dims;

    bool fromJSONFile(QString file_name);

private:
    const QString KEYWORD_TYPE = "type";
    const QString KEYWORD_GRID = "grid";
    const QString KEYWORD_ROWS = "rows";
    const QString KEYWORD_COLUMNS = "columns";
    const QString KEYWORD_DATA = "data";
    const QString KEYWORD_PATH = "path";
    const QString KEYWORD_LENGTH = "length";
    const QString KEYWORD_DIMENSIONS = "dimensions";
    const QString KEYWORD_X = "x";
    const QString KEYWORD_Y = "y";
    const QString KEYWORD_Z = "z";
};

#endif // INPUT_CONFIGURATION_H
