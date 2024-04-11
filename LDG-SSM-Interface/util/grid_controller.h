#ifndef GRID_CONTROLLER_H
#define GRID_CONTROLLER_H

#include "tree_draw_properties.h"

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

    void splitNode(size_t height, size_t index);
    void mergeNode(size_t height, size_t index);

    std::pair<int, int> resolveGridPosition(QPointF &position);

public:
    GridController(TreeDrawProperties *draw_properties);

public slots:
    /**
     * @brief GridController::handleMouseClick Finds the clicked node and splits or merges it if appropriate.
     * @param event
     */
    void handleMouseClick(QMouseEvent *event);
    void selectHeight(size_t height);

signals:
    void gridChanged();
};

#endif // GRID_CONTROLLER_H
