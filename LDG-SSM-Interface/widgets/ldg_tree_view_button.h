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
    Q_OBJECT;

    const int FRAME_STYLE = QFrame::StyledPanel | QFrame::Plain;
    const int SPACING = 4;

    // Dimensions and location at the current button.
    const size_t height;
    const size_t index;
    size_t num_rows;
    size_t num_cols;

    // Hierarchy: Parent and children.
    LDGTreeViewButton *parent_button;
    std::vector<LDGTreeViewButton *> children;

    void setStyleOptions();
    void split();
    void merge();

public:
    explicit LDGTreeViewButton(
        const size_t height,
        const size_t index,
        LDGTreeViewButton *parent = nullptr
    );
    ~LDGTreeViewButton();

    void setParent(LDGTreeViewButton *parent);
    void setChildren(std::vector<LDGTreeViewButton *> children, const size_t num_rows, const size_t num_cols);

    size_t getIndex();
    size_t getHeight();

public slots:
    void selectHeight(size_t height);

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // LDGTREEVIEWBUTTON_H
