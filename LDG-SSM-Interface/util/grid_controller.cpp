#include "grid_controller.h"
#include "tree_functions.h"
#include <QQueue>

/**
 * @brief GridController::GridController
 * @param draw_properties
 */
GridController::GridController(TreeDrawProperties *draw_properties):
    draw_properties(draw_properties)
{
}

/**
 * @brief GridController::splitNode Split the node into 4 children. This function assumes the given node is valid.
 * @param height
 * @param index
 */
void GridController::splitNode(size_t height, size_t index)
{
    draw_properties->draw_array.remove({ height, index });
    for (auto &child_index : getChildrenIndices(height, index, draw_properties)) {
        if (child_index != -1)
            draw_properties->draw_array.insert({ height - 1, child_index });
    }
}

/**
 * @brief GridController::mergeNode Merge the node and all other descendants of the parent into the parent. This function assumes the given node is valid.
 * @param height
 * @param index
 */
void GridController::mergeNode(size_t height, size_t index)
{
    // Unset previous values, including children.
    auto parent_index = getParentIndex(height, index, draw_properties);
    QQueue<std::pair<size_t, int>> queue;
    for (auto &child_index : getChildrenIndices(height + 1, parent_index, draw_properties))
        queue.enqueue({ height, child_index });

    while (!queue.isEmpty()) {
        auto [node_height, node_index] = queue.dequeue();
        if (node_index != -1 && !draw_properties->draw_array.remove({ node_height, node_index })) {
            for (auto &child_index : getChildrenIndices(node_height, node_index, draw_properties))
                queue.enqueue({ node_height - 1, child_index });
        }
    }

    // Finally, add the parent
    draw_properties->draw_array.insert({ height + 1, parent_index });
}

/**
 * @brief GridController::resolveGridPosition Find the grid cell on the specified position. Returns -1 if it didn't find anything.
 * @param position
 * @return
 */
std::pair<int, int> GridController::resolveGridPosition(QPointF &position)
{
    auto side_len = draw_properties->height_node_lens[0] + draw_properties->node_spacing;
    size_t col = std::max(0., std::floor((position.x() - 1 + draw_properties->node_spacing / 2.) / side_len));
    size_t row = std::max(0., std::floor((position.y() - 1 + draw_properties->node_spacing / 2.) / side_len));

    int found_index = -1;
    int found_height = -1;
    for (size_t height = 0; height <= draw_properties->tree_max_height; ++height, col /= 2, row /= 2) {
        auto [num_rows, num_cols] = draw_properties->height_dims[height];
        int possible_index = row * num_cols + col;
        if (col < num_cols && row < num_rows && draw_properties->draw_array.contains({ height, possible_index })) {
            found_index = possible_index;
            found_height = height;
            break;
        }
    }

    return { found_height, found_index };
}

/**
 * @brief GridController::handleMouseClick Finds the clicked node and splits or merges it if appropriate.
 * @param event
 */
void GridController::handleMouseClick(QMouseEvent *event)
{
    if (event->buttons() == Qt::RightButton || event->buttons() == Qt::LeftButton) {
        auto position = event->position();
        auto [height, index] = resolveGridPosition(position);

        // We found the clicked node
        if (index != -1 && height != -1) {
            if (event->buttons() == Qt::RightButton && height < draw_properties->tree_max_height) {
                mergeNode(height, index);
                emit gridChanged();
            }
            if (event->buttons() == Qt::LeftButton && height > 0) {
                splitNode(height, index);
                emit gridChanged();
            }
        }
    }
}

/**
 * @brief GridController::selectHeight
 * @param height
 */
void GridController::selectHeight(size_t height)
{
    if (height <= draw_properties->tree_max_height) {
        auto [num_rows, num_cols] = draw_properties->height_dims[height];

        // Update draw array
        draw_properties->draw_array.clear();
        for (size_t idx = 0; idx < num_rows * num_cols; ++idx) {
            draw_properties->draw_array.insert({ height, idx });
        }

        emit gridChanged();
    }
}
