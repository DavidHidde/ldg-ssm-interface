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
    reset();
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
 * @brief GridController::resolveGridPosition Find the grid cell on the specified position. Returns -1 if it didn't find anything.
 * @param position
 * @return
 */
std::pair<int, int> GridController::resolveGridPosition(QPointF &position)
{
    auto side_len = window_properties->height_node_lens[0] + window_properties->node_spacing;
    size_t col = std::max(0., std::floor((position.x() - 1 + window_properties->node_spacing / 2.) / side_len));
    size_t row = std::max(0., std::floor((position.y() - 1 + window_properties->node_spacing / 2.) / side_len));

    int found_index = -1;
    int found_height = -1;
    for (size_t height = 0; height <= tree_properties->tree_max_height; ++height, col /= 2, row /= 2) {
        auto [num_rows, num_cols] = tree_properties->height_dims[height];
        int possible_index = row * num_cols + col;
        if (col < num_cols && row < num_rows && tree_properties->draw_array.contains({ height, possible_index })) {
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
    if (event->modifiers() != Qt::ControlModifier && (event->buttons() == Qt::RightButton || event->buttons() == Qt::LeftButton)) {
        auto position = event->position();
        auto [height, index] = resolveGridPosition(position);

        // We found the clicked node
        if (index != -1 && height != -1) {
            if (event->buttons() == Qt::RightButton && height < tree_properties->tree_max_height) {
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
 * @brief GridController::mouseMoveEvent Handles the dragging and rotating of the mesh
 * by looking at the mouse movement.
 * @param event Mouse event.
 * @param width
 * @param height
 */
void GridController::handleMouseMoveEvent(QMouseEvent* event)
{
    // Reset drag if we're not dragging
    if (event->buttons() != Qt::LeftButton || event->modifiers() != Qt::ControlModifier) {
        is_dragging = false;
        prev_dragging_position = QVector3D();
        return;
    }

    // Normalize coordinates
    float x_ratio = event->position().x() / window_properties->window_size.x();
    float y_ratio = event->position().y() / window_properties->window_size.y();

    QVector3D mouse_pos = QVector3D(
        (1. - x_ratio) * -1. + x_ratio * 1.,
        y_ratio * -1. + (1. - y_ratio) * 1.,
        0.0
    );

    // Project onto sphere
    float sqr_z = 1.0f - QVector3D::dotProduct(mouse_pos, mouse_pos);
    if (sqr_z > 0) {
        mouse_pos.setZ(std::sqrt(sqr_z));
    } else {
        mouse_pos.normalize();
    }

    // Reset if we are starting a drag
    if (!is_dragging) {
        is_dragging = true;
        prev_dragging_position = mouse_pos;
        return;
    }

    // Calculate axis and angle
    QVector3D new_pos = mouse_pos.normalized();
    QVector3D last_pos = prev_dragging_position.normalized();
    QVector3D axis = QVector3D::crossProduct(last_pos, new_pos);
    if (axis.length() == 0.0f) {
        prev_dragging_position = mouse_pos;
        return;
    }
    float angle = 180.f / M_PI * std::acos(QVector3D::dotProduct(last_pos, new_pos));
    rotation = QQuaternion::fromAxisAndAngle(rotation.rotatedVector(axis), angle) * rotation;
    prev_dragging_position = mouse_pos;

    updateTransformations();
    emit transformationChanged();
}

/**
 * @brief GridController::handleMouseScrollEvent Move the camera when scrolling
 * @param event
 */
void GridController::handleMouseScrollEvent(QWheelEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier) {
        float phi = event->angleDelta().y() / 200.0f;
        translation.setZ(std::min(translation.z() + phi, 3.5f));
        updateTransformations();
        emit transformationChanged();;
    }
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

/**
 * @brief GridController::reset Reset the transformations.
 */
void GridController::reset()
{
    rotation = QQuaternion::fromEulerAngles(QVector3D{ 270, 0, 0 });
    translation = QVector3D{ 0., 0., 0. };
    updateTransformations();
    emit transformationChanged();
}

/**
 * @brief GridController::updateTransformations Update the model view matrix
 */
void GridController::updateTransformations()
{
    volume_properties->camera_view_transformation.setToIdentity();
    volume_properties->camera_view_transformation.rotate(rotation);
    volume_properties->camera_view_transformation.translate(translation);
}
