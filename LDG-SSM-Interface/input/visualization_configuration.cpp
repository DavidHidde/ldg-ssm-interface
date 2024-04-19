#include "visualization_configuration.h"
#include "input/json.h"

/**
 * @brief VisualizationConfiguration::fromJSONFile
 * @param file_name
 * @return
 */
bool VisualizationConfiguration::fromJSONFile(QString file_name)
{
    auto [json, success] = readJSONFromFile(file_name);
    if (!success)
        return false;

    if (!json.contains(KEYWORD_ASSIGNMENT)) {
        qDebug() << "Missing keyword" << KEYWORD_ASSIGNMENT << "in visualization config.";
        return false;
    }
    assignment_path = json[KEYWORD_ASSIGNMENT].toString();

    if (!json.contains(KEYWORD_DISPARITY_CONFIG)) {
        qDebug() << "Missing keyword" << KEYWORD_DISPARITY_CONFIG << "in visualization config.";
        return false;
    }
    disparity_config_path = json[KEYWORD_DISPARITY_CONFIG].toString();

    if (!json.contains(KEYWORD_VISUALIZATION_CONFIG)) {
        qDebug() << "Missing keyword" << KEYWORD_VISUALIZATION_CONFIG << "in visualization config.";
        return false;
    }
    visualization_config_path = json[KEYWORD_VISUALIZATION_CONFIG].toString();

    return true;
}
