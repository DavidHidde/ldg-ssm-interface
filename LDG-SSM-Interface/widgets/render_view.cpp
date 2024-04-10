#include "render_view.h"

#include <QLoggingCategory>
#include <QPainter>

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
    debug_logger.stopLogging();

    delete draw_properties;
}

/**
 * @brief RenderView::initializeGL Initialize the OpenLG window.
 */
void RenderView::initializeGL()
{
    initializeOpenGLFunctions();

    // Setup logging
    QObject::connect(
        &debug_logger,
        SIGNAL(messageLogged(QOpenGLDebugMessage)),
        this,
        SLOT(onGLMessageLogged(QOpenGLDebugMessage)),
        Qt::DirectConnection
    );

    if (debug_logger.initialize()) {
        QLoggingCategory::setFilterRules(
            "qt.*=false\n"
            "qt.text.font.*=false"
        );
        debug_logger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
        debug_logger.enableMessages();
    }

    makeCurrent();
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glClearColor(1., 1., 1., 1.0);
}

/**
 * @brief RenderView::paintGL Draw the OpenGL scene.
 */
void RenderView::paintGL()
{
    OverlayPainter painter(this, draw_properties);
    painter.beginNativePainting();  // Begin OpenGL calls ---

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    painter.endNativePainting();    // End OpenGL calls ---

    // Draw the grid overlay
    painter.drawOverlay();
}

/**
 * @brief RenderView::resizeGL Update viewport and draw properties on resize.
 * @param width
 * @param height
 */
void RenderView::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);

    // Update draw properties
    for (int curr_height = draw_properties->tree_max_height; curr_height >= 0; --curr_height) {
        draw_properties->height_node_lens[curr_height] = width;
        width = std::round(static_cast<double>(width - draw_properties->node_spacing) / 2.);
    }
}

/**
 * @brief RenderView::onGLMessageLogged Pipe OpenGL debug messages to debug output.
 * @param message
 */
void RenderView::onGLMessageLogged(QOpenGLDebugMessage message)
{
    qDebug() << "OpenGL: " << message;
}
