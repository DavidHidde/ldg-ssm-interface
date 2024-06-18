#ifndef GRID_CONTROLLER_H
#define GRID_CONTROLLER_H

#include "drawing/model/tree_draw_properties.h"

#include <QObject>
#include <QMouseEvent>
#include <QPoint>

#include <drawing/model/volume_draw_properties.h>
#include <drawing/model/window_draw_properties.h>


/**
 * @brief The GridController class Controller class handling all grid actions.
 */
class GridController: public QObject
{
    Q_OBJECT;

    TreeDrawProperties *tree_properties;
    WindowDrawProperties *window_properties;
    VolumeDrawProperties *volume_properties;

public:
    GridController(TreeDrawProperties *draw_properties, WindowDrawProperties *window_properties, VolumeDrawProperties *volume_properties);

    void splitNode(size_t height, size_t index);
    void mergeNode(size_t height, size_t index, QSet<QPair<size_t, size_t>> *nodes_merged);

public slots:
    void selectHeight(size_t height);
    void selectDisparity(double disparity);

signals:
    void gridChanged();
};

#endif // GRID_CONTROLLER_H
