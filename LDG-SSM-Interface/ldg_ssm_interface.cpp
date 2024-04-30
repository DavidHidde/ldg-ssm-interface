#include "ldg_ssm_interface.h"
#include "./ui_ldg_ssm_interface.h"
#include "QtGui/qevent.h"
#include "input/data_buffer.h"
#include "drawing/image_renderer.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QWindow>
#include <drawing/volume_raycaster.h>

/**
 * @brief LDGSSMInterface::LDGSSMInterface
 * @param parent
 */
LDGSSMInterface::LDGSSMInterface(QWidget *parent):
    render_view(nullptr),
    QMainWindow(parent),
    ui(new Ui::LDGSSMInterface)
{
    ui->setupUi(this);
    setWindowTitle("LDG-SSM");
    scroll_area = ui->scrollArea;
    initializeMenus();

    // Begin by asking to open a file
    openFile();
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
 * @brief LDGSSMInterface::openFile
 */
void LDGSSMInterface::openFile()
{
    // Remove current renderer if it exists
    if (render_view != nullptr) {
        scroll_area->takeWidget();
        render_view->deleteLater();
        render_view = nullptr;
    }

    // Get the file
    // QString file_name = QFileDialog::getOpenFileName(this, tr("Select config"), "", tr("Config Files (*.json)"));

    auto [data_map, tree_properties] = readInput(file_name);
    tree_properties->device_pixel_ratio = static_cast<float>(devicePixelRatio());
    if (data_map == nullptr || tree_properties == nullptr) {
        QMessageBox msg_box;
        msg_box.setText("The config couldn't be loaded.");
        msg_box.exec();
        return;
    }

    VolumeDrawProperties *volume_properties = new VolumeDrawProperties();
    volume_properties->camera_view_transformation.setToIdentity();
    volume_properties->color_0 = { 0, 0, 1 };
    volume_properties->color_1 = { 1, 1, 1 };
    volume_properties->color_2 = { 1, 0, 0 };
    volume_properties->render_type = VolumeRenderingType::MAX;
    volume_properties->sample_steps = 100;
    GridController *grid_controller = new GridController(tree_properties, volume_properties);
    VolumeRaycaster *renderer = new VolumeRaycaster(tree_properties, volume_properties);
    // ImageRenderer *renderer = new ImageRenderer(tree_properties, data_map);

    render_view = new RenderView(scroll_area, tree_properties, grid_controller, renderer);
    QObject::connect(this, &LDGSSMInterface::selectionChanged, grid_controller, &GridController::selectHeight);
    QObject::connect(window()->windowHandle(), &QWindow::screenChanged, render_view, &RenderView::screenChanged);

    QPalette palette;
    palette.setColor(QPalette::Window, QColor{
        static_cast<int>(std::round(tree_properties->background_color.x() * 255)),
        static_cast<int>(std::round(tree_properties->background_color.y() * 255)),
        static_cast<int>(std::round(tree_properties->background_color.z() * 255))
    });
    centralWidget()->setPalette(palette);
    scroll_area->setWidget(render_view);
}

/**
 * @brief LDGSSMInterface::resetView Reset the view to not zoomed in and reset the transformations.
 */
void LDGSSMInterface::resetView()
{
    render_view->resetView();
    scroll_area->fitWindow();
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
    file_menu->addAction("Open", QKeySequence::Open, this, &LDGSSMInterface::openFile);

    // Initialize the view menu shortcuts.
    view_menu = ui->menuView;
    view_menu->addAction("Reset view", Qt::Key_R, this, &LDGSSMInterface::resetView);
    view_menu->addAction("Zoom in", QKeySequence::ZoomIn, scroll_area, &PannableScrollArea::zoomIn);
    view_menu->addAction("Zoom out", QKeySequence::ZoomOut, scroll_area, &PannableScrollArea::zoomOut);
}
