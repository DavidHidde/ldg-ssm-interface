#include "tree_functions.h"

/**
 * @brief getParentIndex Get the index of the parent of the specified node.
 * @param height
 * @param index
 * @param tree_properties
 * @return
 */
size_t getParentIndex(size_t height, size_t index, TreeDrawProperties *tree_properties)
{
    auto [num_rows, num_cols] = tree_properties->height_dims[height];
    size_t row = index / (2 * num_cols);
    size_t col = (index % num_cols) / 2;
    num_cols = num_cols / 2 + num_cols % 2;
    return row * num_cols + col;
}

/**
 * @brief getChildrenIndices Get the indices of the children of the specified node. -1 means they don't exist.
 * @param height
 * @param index
 * @param tree_properties
 * @return
 */
std::array<int, 4> getChildrenIndices(size_t height, size_t index, TreeDrawProperties *tree_properties)
{
    auto [num_rows, num_cols] = tree_properties->height_dims[height];
    auto [child_num_rows, child_num_cols] = tree_properties->height_dims[height - 1];

    int row = (index / num_cols) * 2;
    int col = (index % num_cols) * 2;
    int new_index = row * child_num_cols + col;
    return {
        new_index,
        col + 1 < child_num_cols ? new_index + 1 : -1,
        row + 1 < child_num_rows ? new_index + static_cast<int>(child_num_cols) : -1,
        col + 1 < child_num_cols && row + 1 < child_num_rows ? new_index + static_cast<int>(child_num_cols) + 1 : -1
    };
}
