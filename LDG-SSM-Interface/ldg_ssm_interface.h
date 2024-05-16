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

    PannableScrollArea *scroll_area;
    RenderView *render_view;

    TreeDrawProperties *tree_properties;
    WindowDrawProperties *window_properties;
    VolumeDrawProperties *volume_properties;

    QMenu *file_menu;
    QMenu *view_menu;

    void initializeMenus();
    void initializeUI();

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
    void on_transferFunctionColor2Button_clicked();
    void on_transferFunctionColor1Button_clicked();
    void on_transferFunctionColor0Button_clicked();
    void on_thresholdSlider_valueChanged(int value);
    void on_thresholdSpinBox_valueChanged(double value);

protected:
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void selectionChanged(size_t height);

};
#endif // LDGSSMINTERFACE_H
