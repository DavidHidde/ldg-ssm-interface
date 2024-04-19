#include "render_view.h"

#include <QLoggingCategory>
#include <QMatrix4x4>
#include <QPainter>
#include <QOpenGLVersionFunctionsFactory>

/**
 * @brief RenderView::RenderView
 * @param parent
 * @param draw_properties
 * @param grid_controller
 */
RenderView::RenderView(
    QWidget *parent,
    TreeDrawProperties *draw_properties,
    GridController *grid_controller,
    Renderer *renderer
):
    draw_properties(draw_properties),
    grid_controller(grid_controller),
    renderer(renderer),
    QOpenGLWidget(parent)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    QObject::connect(grid_controller, &GridController::gridChanged, this, &RenderView::update);
}

/**
 * @brief RenderView::~RenderView
 */
RenderView::~RenderView()
{
    debug_logger.stopLogging();

    delete renderer;
    delete grid_controller;
    delete draw_properties;
}

/**
 * @brief RenderView::initializeGL Initialize the OpenGL functions, logging and renderer.
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
    renderer->intialize(QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_1_Core>(this->context()));
}

/**
 * @brief RenderView::paintGL Draw the OpenGL scene, which is delegated to the overlay painter and renderer.
 */
void RenderView::paintGL()
{
    OverlayPainter painter(this, draw_properties);
    painter.beginNativePainting();  // Begin OpenGL calls ---
    renderer->render();
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
    // Update drawing properties.
    double side_len = width - 2; // Subtract 2 pixels to make sure we stay within the borders
    for (int curr_height = draw_properties->tree_max_height; curr_height >= 0; --curr_height) {
        draw_properties->height_node_lens[curr_height] = side_len;
        side_len = (side_len - draw_properties->node_spacing) / 2.;
    }
    draw_properties->gl_space_scale_vector = QVector3D{2.f / static_cast<float>(width), 2.f / static_cast<float>(height), 1.f };

    // Update renderer
    glViewport(0, 0, width, height);
    draw_properties->viewport = QRect{ 0, 0, width, height };
    renderer->updateUniforms();
    renderer->updateBuffers();
}

/**
 * @brief RenderView::mousePressEvent
 * @param event
 */
void RenderView::mousePressEvent(QMouseEvent *event)
{
    grid_controller->handleMouseClick(event);
}

/**
 * @brief RenderView::onGLMessageLogged Pipe OpenGL debug messages to debug output.
 * @param message
 */
void RenderView::onGLMessageLogged(QOpenGLDebugMessage message)
{
    qDebug() << "OpenGL: " << message;
}

/**
 * @brief RenderView::repaint Repaint slot since the build-in one doesn't seem to work.
 */
void RenderView::update()
{
    renderer->updateBuffers();
    QOpenGLWidget::update();
}
