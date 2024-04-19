#ifndef VISUALIZATION_CONFIGURATION_H
#define VISUALIZATION_CONFIGURATION_H

#include <QString>

/**
 * Simple configuration for export which points to an assignment, its disparity and its visualization
 */
struct VisualizationConfiguration
{
    QString assignment_path;
    QString disparity_config_path;
    QString visualization_config_path;

    bool fromJSONFile(QString file_name);

private:
    const QString KEYWORD_ASSIGNMENT = "assignment";
    const QString KEYWORD_DISPARITY_CONFIG = "disparity_config";
    const QString KEYWORD_VISUALIZATION_CONFIG = "visualization_config";
};

#endif // VISUALIZATION_CONFIGURATION_H
