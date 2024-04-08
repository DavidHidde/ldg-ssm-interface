#include "build_tree.h"

/**
 * @brief getChildrenIndices Get the child indices given a parent index. Returns -1 for non-existant children.
 * @param parent_idx
 * @param parent_num_cols
 * @param child_num_rows
 * @param child_num_cols
 * @return
 */
std::array<int, 4> getChildrenIndices(size_t parent_idx, size_t parent_num_cols, int child_num_rows, int child_num_cols)
{
    int child_row = (parent_idx / parent_num_cols) * 2;
    int child_col = (parent_idx % parent_num_cols) * 2;

    int index = static_cast<int>(child_row * child_num_cols + child_col);
    return {
        index,                                                                                              // North-west
        child_col + 1 < child_num_cols ? index + 1 : -1,                                                    // North-east
        child_row + 1 < child_num_rows ? index + child_num_cols : -1,                                       // South-west
        child_col + 1 < child_num_cols && child_row + 1 < child_num_rows ? index + child_num_cols + 1 : -1  // South-east
    };
}

/**
 * @brief ceilDividePow2 Get the ceiling of the power of two of the number.
 * @param num
 * @param pow
 * @return
 */
size_t ceilDividePow2(double num, size_t pow)
{
    return static_cast<size_t>(std::ceil(num / std::pow(2., pow)));
}


/**
 * @brief buildTree Build a tree top-down given the dimensions.
 * @param num_rows
 * @param num_cols
 * @param parent
 * @return
 */
LDGTreeViewButton *buildTree(size_t num_rows, size_t num_cols, LDGSSMInterface *parent)
{
    size_t height = static_cast<size_t>(ceil(log(num_rows * num_cols) / log(4)));


    LDGTreeViewButton *root = new LDGTreeViewButton{ height, 0 };
    QObject::connect(parent, &LDGSSMInterface::selectionChanged, root, &LDGTreeViewButton::selectHeight);
    std::vector<LDGTreeViewButton *> parent_queue{ root };
    std::vector<LDGTreeViewButton *> children_queue;
    size_t parent_height_num_cols = 1;

    for (; height > 0; --height) {
        size_t child_height_num_rows = ceilDividePow2(num_rows, height - 1);
        size_t child_height_num_cols = ceilDividePow2(num_cols, height - 1);

        for (auto *parent_button : parent_queue) {
            std::vector<LDGTreeViewButton *> child_container;
            auto child_indices = getChildrenIndices(parent_button->getIndex(), parent_height_num_cols, child_height_num_rows, child_height_num_cols);
            for (auto child_idx : child_indices) {
                if (child_idx != -1) {
                    LDGTreeViewButton *child_button = new LDGTreeViewButton(height - 1, child_idx, parent_button);
                    QObject::connect(parent, &LDGSSMInterface::selectionChanged, child_button, &LDGTreeViewButton::selectHeight);
                    child_container.push_back(child_button);
                    children_queue.push_back(child_button);
                }
            }
            parent_button->setChildren(child_container, child_indices[0] != -1 && child_indices[2] != -1 ? 2 : 1, child_indices[0] != -1 && child_indices[1] != -1 ? 2 : 1);
        }
        parent_queue.clear();
        std::swap(parent_queue, children_queue);
        parent_height_num_cols = child_height_num_cols;
    }

    return root;
}
