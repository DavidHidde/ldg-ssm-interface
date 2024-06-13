#ifndef LDGSSMINTERFACE_H
#define LDGSSMINTERFACE_H

#include <QMainWindow>

#include "widgets/render_view.h"
#include "widgets/pannable_scroll_area.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class LDGSSMInterface;
}
QT_END_NAMESPACE

/**
 * @brief The LDGSSMInterface class Main window of the application.
 */
class LDGSSMInterface: public QMainWindow
{
    Q_OBJECT

    Ui::LDGSSMInterface *ui;

    // Model
    TreeDrawProperties *tree_properties = nullptr;
    WindowDrawProperties *window_properties = nullptr;
    VolumeDrawProperties *volume_properties = nullptr;

    // View
    RenderView *render_view = nullptr;

    // Controller
    PannableScrollArea *scroll_area = nullptr;
    GridController *grid_controller = nullptr;

    QMenu *file_menu;
    QMenu *view_menu;

    void initializeMenus();
    void initializeUI();
    void initializeModelController();
    bool isReady();

public:
    LDGSSMInterface(QWidget *parent = nullptr);    
    ~LDGSSMInterface();

public slots:
    void openFile();
    void resetView();

private slots:
    void on_backgroundColorSelectButton_clicked();
    void on_heightSpinBox_valueChanged(int value);
    void on_disparitySlider_valueChanged(int value);
    void on_disparitySpinBox_valueChanged(double value);
    void on_renderTypeSelectBox_currentIndexChanged(int index);
    void on_sampleStepsSpinBox_valueChanged(int value);
    void on_thresholdSlider_valueChanged(int value);
    void on_thresholdSpinBox_valueChanged(double value);

protected:
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void heightSelected(size_t height);
    void disparitySelected(double disparity);

};
#endif // LDGSSMINTERFACE_H
