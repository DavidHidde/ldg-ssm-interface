#ifndef SCREEN_CONTROLLER_H
#define SCREEN_CONTROLLER_H

#include "util/grid_controller.h"
#include <QMouseEvent>
#include <QObject>

#include <drawing/model/tree_draw_properties.h>
#include <drawing/model/volume_draw_properties.h>
#include <drawing/model/window_draw_properties.h>

/**
 * @brief The ScreenController class Controller for screen interactions like rotating and zooming
 */
class ScreenController: public QObject
{
    Q_OBJECT

    TreeDrawProperties *tree_properties;
    WindowDrawProperties *window_properties;
    VolumeDrawProperties *volume_properties;

    GridController *grid_controller;

    bool is_dragging = false;
    QVector3D prev_dragging_position;
    QQuaternion rotation;
    QVector3D translation;

    std::pair<int, int> resolveGridPosition(QPointF &position);

public:
    ScreenController(TreeDrawProperties *draw_properties, WindowDrawProperties *window_properties, VolumeDrawProperties *volume_properties, GridController *grid_controller);

    void reset();
    void updateTransformations();

    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event, float screen_width, float screen_height);
    void handleWheelEvent(QWheelEvent *event);

signals:
    void transformationChanged();
    void gridChanged();
};

#endif // SCREEN_CONTROLLER_H
