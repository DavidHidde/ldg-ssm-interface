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

public:
    GridController(TreeDrawProperties *draw_properties);

    QPoint fromWorldSpaceToScreenSpace(QPoint &point);

public slots:
    void handleMouseClick(QMouseEvent *event);
    void selectHeight(size_t height);

signals:
    void gridChanged();
};

#endif // GRID_CONTROLLER_H
