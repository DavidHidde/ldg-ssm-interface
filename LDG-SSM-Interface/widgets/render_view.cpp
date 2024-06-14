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
    cleanup();
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
    QObject::connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &RenderView::cleanup);
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
        gl->glEnable(GL_SCISSOR_TEST);

        auto viewport = window_properties->current_viewport;
        gl->glScissor(
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
 * @brief RenderView::repaint Update the buffers and repaint if they changed.
 */
void RenderView::updateBuffers()
{
    makeCurrent();
    renderer->updateBuffers();
    QOpenGLWidget::update();
}

/**
 * @brief RenderView::updateUniforms Update the uniforms and repaint if they changed.
 */
void RenderView::updateUniforms()
{
    makeCurrent();
    renderer->updateUniforms();
    QOpenGLWidget::update();
}

/**
 * @brief RenderView::cleanup Cleanup all claimed resources.
 */
void RenderView::cleanup()
{
    debug_logger.stopLogging();
    deleteRenderer();
}

void RenderView::deleteRenderer()
{
    makeCurrent();
    delete renderer;
    doneCurrent();
}
