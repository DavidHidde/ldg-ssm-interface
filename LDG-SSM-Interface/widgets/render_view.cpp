#include "render_view.h"

#include <QLoggingCategory>
#include <QMatrix4x4>
#include <QPainter>
#include <QOpenGLVersionFunctionsFactory>

#include <drawing/overlay_painter.h>

/**
 * @brief RenderView::RenderView
 * @param parent
 * @param tree_properties
 * @param window_properties
 * @param grid_controller
 * @param renderer
 */
RenderView::RenderView(
    QWidget *parent,
    TreeDrawProperties *tree_properties,
    WindowDrawProperties *window_properties,
    GridController *grid_controller,
    Renderer *renderer
):
    tree_properties(tree_properties),
    window_properties(window_properties),
    grid_controller(grid_controller),
    renderer(renderer),
    QOpenGLWidget(parent)
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    QObject::connect(grid_controller, &GridController::gridChanged, this, &RenderView::updateBuffers);
    QObject::connect(grid_controller, &GridController::transformationChanged, this, &RenderView::updateUniforms);
}

/**
 * @brief RenderView::~RenderView
 */
RenderView::~RenderView()
{
    debug_logger.stopLogging();

    delete renderer;
    delete grid_controller;
    delete tree_properties;
    delete window_properties;
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
    OverlayPainter painter(this, tree_properties, window_properties);
    painter.beginNativePainting();  // Begin OpenGL calls ---
    glEnable(GL_SCISSOR_TEST);

    auto viewport = window_properties->current_viewport;
    glScissor(
        viewport.left() * window_properties->device_pixel_ratio,
        (window_properties->window_size.y() - viewport.top() - viewport.height()) * window_properties->device_pixel_ratio,  // Translate top-left origin to bot-left
        viewport.width() * window_properties->device_pixel_ratio,
        viewport.height() * window_properties->device_pixel_ratio
    );

    renderer->render(); // Render content
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
    float opengl_width = width * window_properties->device_pixel_ratio;
    float opengl_height = height * window_properties->device_pixel_ratio;
    double side_len = width - 2; // Subtract 2 pixels to make sure we stay within the borders
    for (int curr_height = tree_properties->tree_max_height; curr_height >= 0; --curr_height) {
        window_properties->height_node_lens[curr_height] = side_len;
        side_len = (side_len - window_properties->node_spacing) / 2.;
    }
    tree_properties->gl_space_scale_vector = QVector3D{
        2.f / opengl_width,
        2.f / opengl_height,
        1.f
    };

    // Update renderer
    glViewport(0, 0, opengl_width, opengl_height);
    window_properties->window_size = { static_cast<float>(width), static_cast<float>(height) };
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
 * @brief RenderView::mouseMoveEvent
 * @param event
 */
void RenderView::mouseMoveEvent(QMouseEvent *event)
{
    grid_controller->handleMouseMoveEvent(event);
}

/**
 * @brief RenderView::wheelEvent
 * @param event
 */
void RenderView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier)
        grid_controller->handleMouseScrollEvent(event);
    else
        QOpenGLWidget::wheelEvent(event);
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
 * @brief RenderView::repaint Update the buffers and repaint if they changed.
 */
void RenderView::updateBuffers()
{
    renderer->updateBuffers();
    QOpenGLWidget::update();
}

/**
 * @brief RenderView::updateUniforms Update the uniforms and repaint if they changed.
 */
void RenderView::updateUniforms()
{
    renderer->updateUniforms();
    QOpenGLWidget::update();
}

/**
 * @brief RenderView::resetView
 */
void RenderView::resetView()
{
    grid_controller->reset();
}

/**
 * @brief RenderView::screenChanged Update the device pixel ratio when switching screens
 */
void RenderView::screenChanged()
{
    window_properties->device_pixel_ratio = devicePixelRatio();
    resizeGL(width(), height());
    update();
}
