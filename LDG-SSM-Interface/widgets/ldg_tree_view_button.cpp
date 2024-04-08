#include "ldg_tree_view_button.h"
#include "../layout/square_grid_layout.h"

/**
 * @brief LDGTreeViewButton::LDGTreeViewButton Create a
 * @param height
 * @param children The children this class manages. The indices of the children must be row-major according to the rows and columns.
 * @param parent
 */
LDGTreeViewButton::LDGTreeViewButton(
    const size_t height,
    const size_t index,
    LDGTreeViewButton *parent
):
    height(height),
    index(index),
    parent_button(parent),
    QFrame{ parent }
{
    setStyleOptions();
}

/**
 * @brief LDGTreeViewButton::~LDGTreeViewButton
 */
LDGTreeViewButton::~LDGTreeViewButton()
{
    for (auto *child : children)
        delete child;
    delete layout();
}

/**
 * @brief LDGTreeViewButton::setParent
 * @param parent
 */
void LDGTreeViewButton::setParent(LDGTreeViewButton *parent)
{
    parent_button = parent;
    QFrame::setParent(parent);
}

/**
 * @brief LDGTreeViewButton::setChildren
 * @param children
 * @param num_rows
 * @param num_cols
 */
void LDGTreeViewButton::setChildren(std::vector<LDGTreeViewButton *> children, const size_t num_rows, const size_t num_cols)
{
    // Remove current children and layout first
    for (auto *child : this->children)
        delete child;
    delete layout();

    // Add new children and layout
    this->num_rows = num_rows;
    this->num_cols = num_cols;

    SquareGridLayout *layout = new SquareGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(SPACING);
    setLayout(layout);

    for (size_t y = 0; y < 2; ++y) {
        for (size_t x = 0; x < 2; ++x) {
            if (x >= num_cols || y >= num_rows) { // Just add empty frames to fill the void.
                layout->addWidget(new QFrame{ this }, y, x);
            } else {
                auto *button = children[y * num_cols + x];
                button->setVisible(false);
                layout->addWidget(button, y, x);
                this->children.push_back(button);
            }
        }
    }
}

/**
 * @brief LDGTreeViewButton::index
 * @return
 */
size_t LDGTreeViewButton::getIndex()
{
    return index;
}

/**
 * @brief LDGTreeViewButton::height
 * @return
 */
size_t LDGTreeViewButton::getHeight()
{
    return height;
}

/**
 * @brief mousePressEvent
 * @param event
 */
void LDGTreeViewButton::mousePressEvent(QMouseEvent *event)
{
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
            break;
    }
}

/**
 * @brief LDGTreeViewButton::selectConditionally Split/merge or do nothing based on a selected height.
 * @param height
 * @param disparity
 */
void LDGTreeViewButton::selectHeight(size_t height)
{
    if (this->height == height) {   // Merge all children into this node.
        merge();
    }
    if (this->height > height) {    // Split this node such that the lower height is visible.
        split();
    }
    // If both don't trigger (below height), the node got automatically merged.
}

/**
 * @brief LDGTreeViewButton::split Split this button up into its children
 */
void LDGTreeViewButton::split()
{
    if (!children.empty()) {
        for (LDGTreeViewButton *button : children) {
            button->setVisible(true);
        }
        setFrameShape(QFrame::NoFrame);
    }
};

/**
 * @brief LDGTreeViewButton::merge Merge the children of this button into this button.
 */
void LDGTreeViewButton::merge()
{
    if (!children.empty()) {
        for (LDGTreeViewButton *button : children) {
            button->merge();
            button->setVisible(false);
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
