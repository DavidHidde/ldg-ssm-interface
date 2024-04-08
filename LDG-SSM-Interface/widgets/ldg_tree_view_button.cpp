#include "ldg_tree_view_button.h"

/**
 * @brief LDGTreeViewButton::LDGTreeViewButton
 * @param parent
 */
LDGTreeViewButton::LDGTreeViewButton(QWidget *parent)
    : QFrame{parent}
{
    setStyleOptions();
}

/**
 * @brief LDGTreeViewButton::heightForWidth Keep a square aspect ratio.
 * @param width
 * @return
 */
int LDGTreeViewButton::heightForWidth(int w) const
{
    qDebug() << "Called height for width";
    return w;
}

/**
 * @brief LDGTreeViewButton::~LDGTreeViewButton
 */
LDGTreeViewButton::~LDGTreeViewButton()
{
    delete children;

}

/**
 * @brief mousePressEvent
 * @param event
 */
void LDGTreeViewButton::mousePressEvent(QMouseEvent *event)
{
    if (children == nullptr)
        setChildren();

    switch (event->buttons()) {
    case Qt::RightButton:
        if (parent_button != nullptr) {
            parent_button->merge();
            update();
        }
        break;
    case Qt::LeftButton:
        split();
        update();
    }
}

/**
 * @brief LDGTreeViewButton::split
 */
void LDGTreeViewButton::split()
{
    if (children != nullptr) {
        for (LDGTreeViewButton &button : *children) {
            button.setVisible(true);
        }
        setFrameShape(QFrame::NoFrame);
    }
};

/**
 * @brief LDGTreeViewButton::merge
 */
void LDGTreeViewButton::merge()
{
    if (children != nullptr) {
        for (LDGTreeViewButton &button : *children) {
            button.merge();
            button.setVisible(false);
        }
        setFrameStyle(FRAME_STYLE);
    }
};

/**
 * @brief LDGTreeViewButton::setStyleOptions
 */
void LDGTreeViewButton::setStyleOptions()
{
    setFrameStyle(FRAME_STYLE);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    auto widget_size = size();
    auto max_dim = widget_size.height() > widget_size.width() ? widget_size.width() : widget_size.height();
    resize(max_dim, max_dim);
}

/**
 * @brief LDGTreeViewButton::setChildren
 */
void LDGTreeViewButton::setChildren()
{
    SquareGridLayout *layout = new SquareGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(SPACING);
    setLayout(layout);

    int idx = 0;
    children = new std::array<LDGTreeViewButton, 4>();
    for (LDGTreeViewButton &button : *children) {
        button.setParent(this);
        button.parent_button = this;
        button.setVisible(false);
        layout->addWidget(&button, idx / 2, idx % 2);
        ++idx;
    }
}
