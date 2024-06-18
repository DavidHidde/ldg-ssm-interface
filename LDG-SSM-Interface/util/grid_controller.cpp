#include "grid_controller.h"
#include "tree_functions.h"
#include <QQueue>

/**
 * @brief GridController::GridController
 * @param tree_properties
 * @param window_properties
 * @param volume_properties
 */
GridController::GridController(TreeDrawProperties *tree_properties, WindowDrawProperties *window_properties, VolumeDrawProperties *volume_properties):
    tree_properties(tree_properties),
    window_properties(window_properties),
    volume_properties(volume_properties)
{
}

/**
 * @brief GridController::splitNode Split the node into 4 children. This function assumes the given node is valid.
 * @param height
 * @param index
 */
void GridController::splitNode(size_t height, size_t index)
{
    tree_properties->draw_array.remove({ height, index });
    for (auto &child_index : getChildrenIndices(height, index, tree_properties)) {
        if (child_index != -1 && !tree_properties->invalid_nodes.contains({ height - 1, child_index }))
            tree_properties->draw_array.insert({ height - 1, child_index });
    }
}

/**
 * @brief GridController::mergeNode Merge the node and all other descendants of the parent into the parent. This function assumes the given node is valid.
 * @param height
 * @param index
 * @param nodes_merged
 */
void GridController::mergeNode(size_t height, size_t index, QSet<QPair<size_t, size_t>> *nodes_merged = nullptr)
{
    // Unset previous values, including children.
    auto parent_index = getParentIndex(height, index, tree_properties);
    QQueue<std::pair<size_t, int>> queue;
    for (auto &child_index : getChildrenIndices(height + 1, parent_index, tree_properties))
        queue.enqueue({ height, child_index });

    while (!queue.isEmpty()) {
        auto [node_height, node_index] = queue.dequeue();
        bool is_removed = node_index != -1 && tree_properties->draw_array.remove({ node_height, node_index });

        if (
            !is_removed &&
            node_index != -1 &&
            node_height > 0 &&
            !tree_properties->invalid_nodes.contains({ node_height, node_index })
        ) {
            for (auto &child_index : getChildrenIndices(node_height, node_index, tree_properties))
                queue.enqueue({ node_height - 1, child_index });
        }

        // Keep track of the nodes that have been merged
        if (is_removed && nodes_merged != nullptr)
            nodes_merged->insert({ node_height, node_index });
    }

    // Finally, add the parent
    tree_properties->draw_array.insert({ height + 1, parent_index });
}

/**
 * @brief GridController::selectHeight
 * @param height
 */
void GridController::selectHeight(size_t height)
{
    if (height <= tree_properties->tree_max_height) {
        auto [num_rows, num_cols] = tree_properties->height_dims[height];

        // Update draw array
        tree_properties->draw_array.clear();
        for (size_t idx = 0; idx < num_rows * num_cols; ++idx) {
            if (!tree_properties->invalid_nodes.contains({ height, idx }))
                tree_properties->draw_array.insert({ height, idx });
        }

        emit gridChanged();
    }
}

/**
 * @brief GridController::selectDisparity
 * @param disparity
 */
void GridController::selectDisparity(double disparity_threshold)
{
    bool changed = false;
    QSet<QPair<size_t, size_t>> nodes_merged;
    size_t changes;

    do {
        changes = 0;

        for (auto location : tree_properties->draw_array.values()) {
            auto [height, index] = location;
            double node_disparity = tree_properties->disparities[location];

            // If the disparity is bigger than the threshold, consider the children.
            if (node_disparity > disparity_threshold && height > 0) {
                splitNode(height, index);
                ++changes;
            }

            // If the disparity is smaller than the threshold, consider the parents. Check if we haven't already merged this node.
            if (node_disparity < disparity_threshold && height < tree_properties->tree_max_height && !nodes_merged.contains({ height, index })) {
                auto parent_index = getParentIndex(height, index, tree_properties);
                if (tree_properties->disparities[{ height + 1, parent_index }] <= disparity_threshold) {
                    mergeNode(height, index, &nodes_merged);
                    ++changes;
                }
            }
        }
        changed = changed || changes > 0;
    } while (changes > 0);

    if (changed)
        emit gridChanged();
}
