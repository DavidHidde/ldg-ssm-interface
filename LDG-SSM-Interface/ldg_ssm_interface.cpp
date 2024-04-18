#include "ldg_ssm_interface.h"
#include "./ui_ldg_ssm_interface.h"
#include "QtGui/qevent.h"
#include "input/data_buffer.h"
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

    auto [data_map, tree_properties] = readInput("some path");
    if (data_map == nullptr) {
        qDebug() << "No data set";
        data_map = new QMap<QPair<size_t, size_t>, QPair<QImage, double>>();
    }
    GridController *grid_controller = new GridController(tree_properties);
    ImageRenderer *renderer = new ImageRenderer(tree_properties, data_map);
    render_view = new RenderView(scroll_area, tree_properties, grid_controller, renderer);

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
