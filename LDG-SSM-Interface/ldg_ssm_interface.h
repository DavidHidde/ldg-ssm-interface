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

    QMenu *file_menu;
    QMenu *view_menu;

    void initializeMenus();

public:
    LDGSSMInterface(QWidget *parent = nullptr);    
    ~LDGSSMInterface();

protected:
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void selectionChanged(size_t height);
};
#endif // LDGSSMINTERFACE_H
