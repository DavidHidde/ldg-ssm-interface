#ifndef GRID_CONTROLLER_H
#define GRID_CONTROLLER_H

#include "drawing/model/tree_draw_properties.h"

#include <QObject>
#include <QMouseEvent>
#include <QPoint>


/**
 * @brief The GridController class Controller class handling all grid actions.
 */
class GridController: public QObject
{
    Q_OBJECT;

    TreeDrawProperties *draw_properties;

    bool is_dragging = false;
    QVector3D prev_dragging_position;
    QQuaternion rotation;
    QVector3D translation;

    void splitNode(size_t height, size_t index);
    void mergeNode(size_t height, size_t index);

    std::pair<int, int> resolveGridPosition(QPointF &position);

public:
    GridController(TreeDrawProperties *draw_properties);

public slots:
    void handleMouseClick(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event, float width, float height);
    void handleMouseScrollEvent(QWheelEvent *event);

    void selectHeight(size_t height);

    void reset();
    void updateTransformations();

signals:
    void gridChanged();
    void transformationChanged();
};

#endif // GRID_CONTROLLER_H
