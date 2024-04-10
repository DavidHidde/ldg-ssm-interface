#ifndef RENDERVIEW_H
#define RENDERVIEW_H

#include <QOpenGLWidget>

#include "util/tree_draw_properties.h"

/**
 * @brief The RenderView class Main rendering widget. Draws the grid and overlay based on draw properties which it takes ownership of.
 */
class RenderView : public QOpenGLWidget
{
    Q_OBJECT

    TreeDrawProperties *draw_properties;

public:
    RenderView(QWidget *parent, TreeDrawProperties *draw_properties);
    ~RenderView();
};

#endif // RENDERVIEW_H
