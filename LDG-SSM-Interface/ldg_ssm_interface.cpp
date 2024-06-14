#include "ldg_ssm_interface.h"
#include "./ui_ldg_ssm_interface.h"
#include "QtGui/qevent.h"
#include "input/data_buffer.h"
#include "drawing/image_renderer.h"
#include <QColorDialog>
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
    tree_properties(nullptr),
    volume_properties(nullptr),
    window_properties(nullptr),
    QMainWindow(parent),
    ui(new Ui::LDGSSMInterface)
{
    ui->setupUi(this);
    setWindowTitle("LDG-SSM");
    scroll_area = ui->scrollArea;
    initializeMenus();
    initializeModelController();
    initializeUI();
}

/**
 * @brief LDGSSMInterface::~LDGSSMInterface
 */
LDGSSMInterface::~LDGSSMInterface()
{
    delete ui;
    delete render_view;
    delete scroll_area;

    delete tree_properties;
    delete volume_properties;
    delete window_properties;
    delete grid_controller;
}

/**
 * @brief LDGSSMInterface::openFile
 */
void LDGSSMInterface::openFile()
{
    // Get the file
    QString file_name = QFileDialog::getOpenFileName(this, tr("Select config"), "", tr("Config Files (*.json)"));
    if (!readInput(file_name, *tree_properties)) {
        QMessageBox msg_box;
        msg_box.setText("The config couldn't be loaded.");
        msg_box.exec();
        return;
    }
    is_ready = render_view != nullptr && scroll_area != nullptr && grid_controller != nullptr && tree_properties != nullptr && window_properties != nullptr && volume_properties != nullptr;


    // Initialize renderer
    scroll_area->updateWindowProperties();
    render_view->deleteRenderer();
    if (tree_properties->draw_type == DrawType::IMAGE) {
        render_view->setRenderer(new ImageRenderer(tree_properties, window_properties));
    } else {
        render_view->setRenderer(new VolumeRaycaster(tree_properties, window_properties, volume_properties));
    }

    QObject::connect(grid_controller, &GridController::gridChanged, render_view, &RenderView::updateBuffers);
    QObject::connect(grid_controller, &GridController::transformationChanged, render_view, &RenderView::updateUniforms);

    initializeUI();
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

/**
 * @brief LDGSSMInterface::initializeUI Initialize the dynamic UI based on the draw properties.
 */
void LDGSSMInterface::initializeUI()
{
    // Set background color
    QPalette palette;
    palette.setColor(QPalette::Window, {
       static_cast<int>(std::round(tree_properties->background_color.x() * 255)),
       static_cast<int>(std::round(tree_properties->background_color.y() * 255)),
       static_cast<int>(std::round(tree_properties->background_color.z() * 255))
    });
    scroll_area->setPalette(palette);

    // Height spin box
    ui->heightSpinBox->blockSignals(true);
    ui->heightSpinBox->setMaximum(tree_properties->tree_max_height);
    ui->heightSpinBox->setValue(tree_properties->tree_max_height);
    ui->heightSpinBox->blockSignals(false);

    // Disparity slider
    ui->disparitySlider->blockSignals(true);
    ui->disparitySpinBox->blockSignals(true);
    ui->disparitySlider->setValue(100);
    ui->disparitySpinBox->setValue(1.);
    ui->disparitySlider->blockSignals(false);
    ui->disparitySpinBox->blockSignals(false);

    // Volume settings panel
    if (tree_properties->draw_type == DrawType::VOLUME) {
        ui->volumeRenderSettingsPanel->setDisabled(false);

        // Render type
        ui->renderTypeSelectBox->blockSignals(true);
        ui->renderTypeSelectBox->setCurrentIndex(static_cast<int>(volume_properties->render_type));
        ui->renderTypeSelectBox->blockSignals(false);

        // Sample steps
        ui->sampleStepsSpinBox->blockSignals(true);
        ui->sampleStepsSpinBox->setValue(volume_properties->sample_steps);
        ui->sampleStepsSpinBox->blockSignals(false);

        // Threshold
        ui->thresholdSlider->blockSignals(true);
        ui->thresholdSpinBox->blockSignals(true);
        ui->thresholdSlider->setValue(volume_properties->threshold * 100);
        ui->thresholdSpinBox->setValue(volume_properties->threshold);
        ui->thresholdSlider->blockSignals(false);
        ui->thresholdSpinBox->blockSignals(false);

        bool threshold_enabled = volume_properties->render_type != VolumeRenderingType::ISOSURFACE;
        ui->thresholdSlider->setDisabled(threshold_enabled);
        ui->thresholdSpinBox->setDisabled(threshold_enabled);
        ui->thresholdLabel->setDisabled(threshold_enabled);
    } else {
        ui->volumeRenderSettingsPanel->setDisabled(true);
    }
}

/**
 * @brief LDGSSMInterface::initializeModelController Initialize the data model and controller classes
 */
void LDGSSMInterface::initializeModelController()
{
    tree_properties = new TreeDrawProperties();
    volume_properties = new VolumeDrawProperties();

    window_properties = new WindowDrawProperties();
    window_properties->scale = 1.;
    window_properties->node_spacing = 4.; // Standard spacing of 4 pixels
    window_properties->device_pixel_ratio = static_cast<float>(devicePixelRatio());

    grid_controller = new GridController(tree_properties, window_properties, volume_properties);
    render_view = new RenderView(scroll_area, tree_properties, window_properties);

    scroll_area->intialize(window_properties, tree_properties, grid_controller);
    scroll_area->setWidget(render_view);
    QObject::connect(window()->windowHandle(), &QWindow::screenChanged, scroll_area, &PannableScrollArea::screenChanged);
}

/**
 * @brief LDGSSMInterface::resetView Reset the view to not zoomed in and reset the transformations.
 */
void LDGSSMInterface::resetView()
{
    if (is_ready) {
        grid_controller->reset();
        scroll_area->fitWindow();
    }
}

/**
 * @brief LDGSSMInterface::keyPressEvent
 * @param event
 */
void LDGSSMInterface::keyPressEvent(QKeyEvent *event)
{
    auto key_pressed = event->key();
    if (is_ready && key_pressed >= Qt::Key_0 && key_pressed <= Qt::Key_9) {
        grid_controller->selectHeight(key_pressed - Qt::Key_0);
    }
}

/**
 * @brief LDGSSMInterface::on_heightSpinBox_valueChanged
 * @param value
 */
void LDGSSMInterface::on_heightSpinBox_valueChanged(int value)
{
    if (is_ready)
        grid_controller->selectHeight(value);
}

/**
 * @brief LDGSSMInterface::on_backgroundColorSelectButton_clicked
 */
void LDGSSMInterface::on_backgroundColorSelectButton_clicked()
{
    const QColor color = QColorDialog::getColor(Qt::white, this, "Select background color");

    if (is_ready && color.isValid()) {
        tree_properties->background_color = {
            static_cast<float>(color.red()) / 255.f,
            static_cast<float>(color.green()) / 255.f,
            static_cast<float>(color.blue()) / 255.f
        };
        QPalette palette;
        palette.setColor(QPalette::Window, color);
        scroll_area->setPalette(palette);

        render_view->updateUniforms();
    }
}

/**
 * @brief LDGSSMInterface::on_disparitySlider_valueChanged
 * @param value
 */
void LDGSSMInterface::on_disparitySlider_valueChanged(int value)
{
    ui->disparitySpinBox->blockSignals(true);
    ui->disparitySpinBox->setValue(static_cast<float>(value) / 100.);
    ui->disparitySpinBox->blockSignals(false);

    if (is_ready)
        grid_controller->selectDisparity(static_cast<float>(value) / 100.);
}

/**
 * @brief LDGSSMInterface::on_disparitySpinBox_valueChanged
 * @param value
 */
void LDGSSMInterface::on_disparitySpinBox_valueChanged(double value)
{
    ui->disparitySlider->blockSignals(true);
    ui->disparitySlider->setValue(value * 100);
    ui->disparitySlider->blockSignals(false);

    if (is_ready)
        grid_controller->selectDisparity(value);
}

/**
 * @brief LDGSSMInterface::on_renderTypeSelectBox_currentIndexChanged
 * @param index
 */
void LDGSSMInterface::on_renderTypeSelectBox_currentIndexChanged(int index)
{
    if (is_ready) {
        volume_properties->render_type = static_cast<VolumeRenderingType>(index);

        bool threshold_enabled = volume_properties->render_type != VolumeRenderingType::ISOSURFACE;
        ui->thresholdSlider->setDisabled(threshold_enabled);
        ui->thresholdSpinBox->setDisabled(threshold_enabled);
        ui->thresholdLabel->setDisabled(threshold_enabled);

        render_view->updateUniforms();
    }
}

/**
 * @brief LDGSSMInterface::on_sampleStepsSpinBox_valueChanged
 * @param value
 */
void LDGSSMInterface::on_sampleStepsSpinBox_valueChanged(int value)
{
    if (is_ready) {
        volume_properties->sample_steps = value;
        render_view->updateUniforms();
    }
}

/**
 * @brief LDGSSMInterface::on_thresholdSlider_valueChanged
 * @param value
 */
void LDGSSMInterface::on_thresholdSlider_valueChanged(int value)
{
    ui->thresholdSpinBox->blockSignals(true);
    ui->thresholdSpinBox->setValue(static_cast<float>(value) / 100.);
    ui->thresholdSpinBox->blockSignals(false);

    if (is_ready) {
        volume_properties->threshold = static_cast<float>(value) / 100.;
        render_view->updateUniforms();
    }
}

/**
 * @brief LDGSSMInterface::on_thresholdSpinBox_valueChanged
 * @param value
 */
void LDGSSMInterface::on_thresholdSpinBox_valueChanged(double value)
{
    ui->thresholdSlider->blockSignals(true);
    ui->thresholdSlider->setValue(value * 100);
    ui->thresholdSlider->blockSignals(false);

    if (is_ready) {
        volume_properties->threshold = value;
        render_view->updateUniforms();
    }
}
