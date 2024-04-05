#ifndef LDGTREEVIEWBUTTON_H
#define LDGTREEVIEWBUTTON_H

#include <QFrame>
#include <QWidget>
#include <QGridLayout>
#include <QMouseEvent>

/**
 * @brief Button that recursively splits up like a quad tree.
 *  This button functions as a view and controller that calls the underlying model for data.
 */
class LDGTreeViewButton: public QFrame
{
    const int FRAME_STYLE = QFrame::StyledPanel | QFrame::Plain;
    const int SPACING = 4;

    LDGTreeViewButton *parent_button;
    std::array<LDGTreeViewButton, 4> *children;

    void setStyleOptions();
    void setChildren();

public:
    explicit LDGTreeViewButton(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *event) override;

    void split();
    void merge();

    ~LDGTreeViewButton();
signals:
};

#endif // LDGTREEVIEWBUTTON_H
