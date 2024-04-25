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
        if (child_index != -1 && !draw_properties->invalid_nodes.contains({ height - 1, child_index }))
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
        if (
            node_index != -1 &&
            !draw_properties->draw_array.remove({ node_height, node_index }) &&
            !draw_properties->invalid_nodes.contains({ node_height, node_index })
        ) {
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
    if (event->modifiers() != Qt::ControlModifier && (event->buttons() == Qt::RightButton || event->buttons() == Qt::LeftButton)) {
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
 * @brief GridController::mouseMoveEvent Handles the dragging and rotating of the mesh
 * by looking at the mouse movement.
 * @param event Mouse event.
 * @param width
 * @param height
 */
void GridController::handleMouseMoveEvent(QMouseEvent* event, float width, float height)
{
    // Reset drag if we're not dragging
    if (event->buttons() != Qt::LeftButton || event->modifiers() != Qt::ControlModifier) {
        is_dragging = false;
        prev_dragging_position = QVector3D();
    }

    // Normalize coordinates
    float x_ratio = event->position().x() / width;
    float y_ratio = event->position().y() / height;

    QVector3D mouse_pos = QVector3D(
        (1. - x_ratio) * -1. + x_ratio * 1.,
        (1. - y_ratio) * -1. + y_ratio * 1.,
        0.0
        );

    // Project onto sphere
    float sqr_z = 1.0f - QVector3D::dotProduct(mouse_pos, mouse_pos);
    if (sqr_z > 0) {
        mouse_pos.setZ(sqrt(sqr_z));
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
    QVector3D curr_pos = prev_dragging_position.normalized();
    QVector3D normal = QVector3D::crossProduct(curr_pos, new_pos).normalized();
    if (normal.length() == 0.0f) {
        prev_dragging_position = mouse_pos;
        return;
    }
    float angle = 180.0f / M_PI * acos(QVector3D::dotProduct(curr_pos, new_pos));
    rotation *= QQuaternion::fromAxisAndAngle(normal, angle);
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
    if (height <= draw_properties->tree_max_height) {
        auto [num_rows, num_cols] = draw_properties->height_dims[height];

        // Update draw array
        draw_properties->draw_array.clear();
        for (size_t idx = 0; idx < num_rows * num_cols; ++idx) {
            if (!draw_properties->invalid_nodes.contains({ height, idx }))
                draw_properties->draw_array.insert({ height, idx });
        }

        emit gridChanged();
    }
}

/**
 * @brief GridController::reset Reset the transformations.
 */
void GridController::reset()
{
    rotation = QQuaternion{};
    translation = QVector3D{ 0., 0., 0. };
    updateTransformations();
    emit transformationChanged();
}

/**
 * @brief GridController::updateTransformations Update the model view matrix
 */
void GridController::updateTransformations()
{
    draw_properties->model_view.setToIdentity();
    draw_properties->model_view.rotate(rotation);
    draw_properties->model_view.translate(translation);
}
