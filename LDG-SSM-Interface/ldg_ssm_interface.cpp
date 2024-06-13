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
        auto render_widget = scroll_area->takeWidget();
        render_widget->deleteLater();
        render_view = nullptr;
        tree_properties = nullptr;
        volume_properties = nullptr;
    }

    // Get the file
    QString file_name = QFileDialog::getOpenFileName(this, tr("Select config"), "", tr("Config Files (*.json)"));
    tree_properties = readInput(file_name);
    if (tree_properties == nullptr) {
        QMessageBox msg_box;
        msg_box.setText("The config couldn't be loaded.");
        msg_box.exec();
        return;
    }

    // Initialize renderer
    window_properties = new WindowDrawProperties();
    window_properties->scale = 1.;
    window_properties->node_spacing = 4.; // Standard spacing of 4 pixels
    window_properties->device_pixel_ratio = static_cast<float>(devicePixelRatio());
    window_properties->height_node_lens = QList<double>(tree_properties->tree_max_height + 1, 0.);
    scroll_area->setWindowDrawProperties(window_properties);

    volume_properties = new VolumeDrawProperties();
    GridController *grid_controller = new GridController(tree_properties, window_properties, volume_properties);

    if (tree_properties->draw_type == DrawType::IMAGE) {
        ImageRenderer *renderer = new ImageRenderer(tree_properties, window_properties);
        render_view = new RenderView(scroll_area, tree_properties, window_properties, grid_controller, renderer);
    } else {
        VolumeRaycaster *renderer = new VolumeRaycaster(tree_properties, window_properties, volume_properties);
        render_view = new RenderView(scroll_area, tree_properties, window_properties, grid_controller, renderer);
    }

    QObject::connect(this, &LDGSSMInterface::heightSelected, grid_controller, &GridController::selectHeight);
    QObject::connect(this, &LDGSSMInterface::disparitySelected, grid_controller, &GridController::selectDisparity);
    QObject::connect(window()->windowHandle(), &QWindow::screenChanged, render_view, &RenderView::screenChanged);

    initializeUI();
    scroll_area->setWidget(render_view);
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
        emit heightSelected(key_pressed - Qt::Key_0);
    }
}

/**
 * @brief LDGSSMInterface::on_heightSpinBox_valueChanged
 * @param value
 */
void LDGSSMInterface::on_heightSpinBox_valueChanged(int value)
{
    emit heightSelected(value);
}

/**
 * @brief LDGSSMInterface::on_backgroundColorSelectButton_clicked
 */
void LDGSSMInterface::on_backgroundColorSelectButton_clicked()
{
    const QColor color = QColorDialog::getColor(Qt::white, this, "Select background color");

    if (color.isValid() && render_view != nullptr) {
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
    emit disparitySelected(static_cast<float>(value) / 100.);
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
    emit disparitySelected(value);
}

/**
 * @brief LDGSSMInterface::on_renderTypeSelectBox_currentIndexChanged
 * @param index
 */
void LDGSSMInterface::on_renderTypeSelectBox_currentIndexChanged(int index)
{
    volume_properties->render_type = static_cast<VolumeRenderingType>(index);

    bool threshold_enabled = volume_properties->render_type != VolumeRenderingType::ISOSURFACE;
    ui->thresholdSlider->setDisabled(threshold_enabled);
    ui->thresholdSpinBox->setDisabled(threshold_enabled);
    ui->thresholdLabel->setDisabled(threshold_enabled);

    render_view->updateUniforms();
}

/**
 * @brief LDGSSMInterface::on_sampleStepsSpinBox_valueChanged
 * @param value
 */
void LDGSSMInterface::on_sampleStepsSpinBox_valueChanged(int value)
{
    volume_properties->sample_steps = value;
    render_view->updateUniforms();
}

/**
 * @brief LDGSSMInterface::on_thresholdSlider_valueChanged
 * @param value
 */
void LDGSSMInterface::on_thresholdSlider_valueChanged(int value)
{
    volume_properties->threshold = static_cast<float>(value) / 100.;
    ui->thresholdSpinBox->blockSignals(true);
    ui->thresholdSpinBox->setValue(static_cast<float>(value) / 100.);
    ui->thresholdSpinBox->blockSignals(false);
    render_view->updateUniforms();
}

/**
 * @brief LDGSSMInterface::on_thresholdSpinBox_valueChanged
 * @param value
 */
void LDGSSMInterface::on_thresholdSpinBox_valueChanged(double value)
{
    volume_properties->threshold = value;
    ui->thresholdSlider->blockSignals(true);
    ui->thresholdSlider->setValue(value * 100);
    ui->thresholdSlider->blockSignals(false);
    render_view->updateUniforms();
}
