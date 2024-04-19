#include "input_configuration.h"
#include "input/json.h"

/**
 * @brief InputConfiguration::fromJSONFile
 * @param file_name
 * @return
 */
bool InputConfiguration::fromJSONFile(QString file_name)
{
    auto [json, success] = readJSONFromFile(file_name);
    if (!success)
        return false;

    if (!json.contains(KEYWORD_TYPE)) {
        qDebug() << "Missing keyword" << KEYWORD_TYPE << "in input config.";
        return false;
    }
    type = json[KEYWORD_TYPE].toString() == "data" ? InputType::DATA : InputType::VISUALIZATION;

    if (
        !json.contains(KEYWORD_GRID) ||
        !json[KEYWORD_GRID].toObject().contains(KEYWORD_ROWS) ||
        !json[KEYWORD_GRID].toObject().contains(KEYWORD_COLUMNS)
    ) {
        qDebug() << "Missing keywords for object" << KEYWORD_GRID << "in input config.";
        return false;
    }
    auto grid = json[KEYWORD_GRID].toObject();
    grid_dims = { grid[KEYWORD_ROWS].toInt(), grid[KEYWORD_COLUMNS].toInt() };

    if (
        !json.contains(KEYWORD_DATA) ||
        !json[KEYWORD_DATA].toObject().contains(KEYWORD_PATH) ||
        !json[KEYWORD_DATA].toObject().contains(KEYWORD_LENGTH) ||
        !json[KEYWORD_DATA].toObject().contains(KEYWORD_DIMENSIONS) ||
        !json[KEYWORD_DATA].toObject()[KEYWORD_DIMENSIONS].toObject().contains(KEYWORD_X) ||
        !json[KEYWORD_DATA].toObject()[KEYWORD_DIMENSIONS].toObject().contains(KEYWORD_Y) ||
        !json[KEYWORD_DATA].toObject()[KEYWORD_DIMENSIONS].toObject().contains(KEYWORD_Z)
    ) {
        qDebug() << "Missing keywords for object" << KEYWORD_DATA << "in input config.";
        return false;
    }
    auto data = json[KEYWORD_DATA].toObject();
    num_elements = data[KEYWORD_LENGTH].toInt();
    data_path = data[KEYWORD_PATH].toString();

    auto dimensions = data[KEYWORD_DIMENSIONS].toObject();
    data_dims = {
        static_cast<size_t>(dimensions[KEYWORD_X].toInt()),
        static_cast<size_t>(dimensions[KEYWORD_Y].toInt()),
        static_cast<size_t>(dimensions[KEYWORD_Z].toInt())
    };

    return true;
}
