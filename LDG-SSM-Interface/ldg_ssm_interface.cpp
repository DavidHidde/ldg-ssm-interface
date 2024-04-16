#include "ldg_ssm_interface.h"
#include "./ui_ldg_ssm_interface.h"
#include "QtGui/qevent.h"
#include "util/tree_functions.h"
#include "drawing/image_renderer.h"

/**
 * @brief LDGSSMInterface::LDGSSMInterface
 * @param parent
 */
LDGSSMInterface::LDGSSMInterface(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::LDGSSMInterface)
{
    ui->setupUi(this);
    setWindowTitle("LDG-SSM");
    scroll_area = ui->scrollArea;

    auto draw_properties = initializeTreeProperties(80, 128);
    GridController *grid_controller = new GridController(draw_properties);

    QImage doot{":/doot.jpg"};
    QImage frog{":/frog.jpg"};
    int max_dim = std::max(doot.width(), doot.height());
    QMap<std::pair<size_t, size_t>, QImage> *textures = new QMap<std::pair<size_t, size_t>, QImage>();
    (*textures)[{0, 0}] = doot.scaled(max_dim, max_dim, Qt::KeepAspectRatio);
    (*textures)[{0, 1}] = frog.scaled(max_dim, max_dim, Qt::KeepAspectRatio);
    ImageRenderer *renderer = new ImageRenderer(draw_properties, textures);
    render_view = new RenderView(scroll_area, draw_properties, grid_controller, renderer);

    QObject::connect(this, &LDGSSMInterface::selectionChanged, grid_controller, &GridController::selectHeight);

    scroll_area->setWidget(render_view);
    initializeMenus();
}

/**
 * @brief LDGSSMInterface::~LDGSSMInterface
 */
LDGSSMInterface::~LDGSSMInterface()
{
    delete ui;
    delete render_view;
    delete scroll_area;
}

/**
 * @brief LDGSSMInterface::keyPressEvent
 * @param event
 */
void LDGSSMInterface::keyPressEvent(QKeyEvent *event)
{
    auto key_pressed = event->key();
    if (key_pressed >= Qt::Key_0 && key_pressed <= Qt::Key_9) {
        emit selectionChanged(key_pressed - Qt::Key_0);
    }
}

/**
 * @brief LDGSSMInterface::LDGSSMInterface Initialize the menus of the window.
 */
void LDGSSMInterface::initializeMenus()
{
    // Initialize the file menu shortcuts.
    file_menu = ui->menuFile;

    // Initialize the view menu shortcuts.
    view_menu = ui->menuView;
    view_menu->addAction("Reset view", Qt::Key_R, scroll_area, &PannableScrollArea::fitWindow);
    view_menu->addAction("Zoom in", QKeySequence::ZoomIn, scroll_area, &PannableScrollArea::zoomIn);
    view_menu->addAction("Zoom out", QKeySequence::ZoomOut, scroll_area, &PannableScrollArea::zoomOut);
}
