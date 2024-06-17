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
 */
RenderView::RenderView(
    QWidget *parent,
    TreeDrawProperties *tree_properties,
    WindowDrawProperties *window_properties
):
    tree_properties(tree_properties),
    window_properties(window_properties),
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
    deleteRenderer();
}

/**
 * @brief RenderView::setRenderer
 * @param renderer
 */
void RenderView::setRenderer(Renderer *renderer)
{
    this->renderer = renderer;
    makeCurrent();
    renderer->intialize(gl);
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
    gl = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_1_Core>(context());
}

/**
 * @brief RenderView::paintGL Draw the OpenGL scene, which is delegated to the overlay painter and renderer.
 */
void RenderView::paintGL()
{
    gl->glClearColor(tree_properties->background_color.x(), tree_properties->background_color.y(), tree_properties->background_color.z(), 1.0);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (renderer != nullptr) {
        OverlayPainter painter(this, tree_properties, window_properties);
        painter.beginNativePainting();  // Begin OpenGL calls ---
        renderer->render(); // Render content
        painter.endNativePainting();    // End OpenGL calls ---

        // Draw the grid overlay
        painter.drawOverlay();
    }
}

/**
 * @brief RenderView::resizeGL Update viewport and draw properties on resize.
 * @param width
 * @param height
 */
void RenderView::resizeGL(int width, int height)
{
    makeCurrent();
    float opengl_width = width * window_properties->device_pixel_ratio;
    float opengl_height = height * window_properties->device_pixel_ratio;

    gl->glViewport(0, 0, opengl_width, opengl_height);
    if (renderer != nullptr) {
        renderer->updateUniforms();
        renderer->updateBuffers();
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

/**
 * @brief RenderView::repaint Update the uniforms and buffers and repaint.
 */
void RenderView::updateUniformsBuffers()
{
    if (renderer != nullptr) {
        makeCurrent();
        renderer->updateUniforms();
        renderer->updateBuffers();
        QOpenGLWidget::update();
    }
}

/**
 * @brief RenderView::repaint Update the buffers and repaint.
 */
void RenderView::updateBuffers()
{
    if (renderer != nullptr) {
        makeCurrent();
        renderer->updateBuffers();
        QOpenGLWidget::update();
    }
}

/**
 * @brief RenderView::updateUniforms Update the uniforms and repaint.
 */
void RenderView::updateUniforms()
{
    if (renderer != nullptr) {
        makeCurrent();
        renderer->updateUniforms();
        QOpenGLWidget::update();
    }
}

/**
 * @brief RenderView::deleteRenderer
 */
void RenderView::deleteRenderer()
{
    makeCurrent();
    delete renderer;
    doneCurrent();
}
