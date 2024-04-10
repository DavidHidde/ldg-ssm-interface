#include "render_view.h"

/**
 * @brief RenderView::RenderView
 * @param parent
 * @param draw_properties
 */
RenderView::RenderView(QWidget *parent, TreeDrawProperties *draw_properties):
    draw_properties(draw_properties),
    QOpenGLWidget(parent)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

/**
 * @brief RenderView::~RenderView
 */
RenderView::~RenderView()
{
    delete draw_properties;
}
