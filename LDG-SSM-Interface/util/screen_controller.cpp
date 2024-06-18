#include "screen_controller.h"

/**
 * @brief ScreenController::ScreenController
 * @param tree_properties
 * @param window_properties
 * @param volume_properties
 * @param grid_controller
 */
ScreenController::ScreenController(TreeDrawProperties *tree_properties, WindowDrawProperties *window_properties, VolumeDrawProperties *volume_properties, GridController *grid_controller):
    tree_properties(tree_properties),
    window_properties(window_properties),
    volume_properties(volume_properties),
    grid_controller(grid_controller)
{
    reset();
}

/**
 * @brief ScreenController::reset Reset the state of the transformations.
 */
void ScreenController::reset()
{
    rotation = QQuaternion();
    translation = QVector3D{ 0., 0., 0. };
    updateTransformations();
    emit transformationChanged();
}

/**
 * @brief ScreenController::updateTransformations Update the transformations of the volume properties.
 */
void ScreenController::updateTransformations()
{
    volume_properties->camera_view_transformation.setToIdentity();
    volume_properties->camera_view_transformation.rotate(rotation);
    volume_properties->camera_view_transformation = volume_properties->camera_view_transformation.inverted();   // Inverted to account for rotating a camera instead of the object
    volume_properties->camera_view_transformation.translate(translation);
}

/**
 * @brief ScreenController::resolveGridPosition Find the grid cell on the specified position. Returns -1 if it didn't find anything.
 * @param position
 * @return
 */
std::pair<int, int> ScreenController::resolveGridPosition(QPointF &position)
{
    auto side_len = window_properties->height_node_lens[0] + window_properties->node_spacing;
    size_t col = std::max(0., std::floor((position.x() - window_properties->draw_origin.x() - 1 + window_properties->node_spacing / 2.) / side_len));
    size_t row = std::max(0., std::floor((position.y() - window_properties->draw_origin.y() - 1 + window_properties->node_spacing / 2.) / side_len));

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
 * @brief ScreenController::handleMousePressEvent Determine if a node has been clicked and split/merge it depending on its location and the button clicked.
 * @param event
 */
void ScreenController::handleMousePressEvent(QMouseEvent *event)
{
    if (event->modifiers() != Qt::ControlModifier && (event->buttons() == Qt::RightButton || event->buttons() == Qt::LeftButton)) {
        auto position = event->position();
        auto [height, index] = resolveGridPosition(position);

        // We found the clicked node
        if (index != -1 && height != -1) {
            if (event->buttons() == Qt::RightButton && height < tree_properties->tree_max_height) {
                grid_controller->mergeNode(height, index, nullptr);
                emit gridChanged();
            }
            if (event->buttons() == Qt::LeftButton && height > 0) {
                grid_controller->splitNode(height, index);
                emit gridChanged();
            }
        }
    }
}

/**
 * @brief ScreenController::handleMouseMoveEvent Handle dragging of the cursor
 *  Partially based on https://www.khronos.org/opengl/wiki/Object_Mouse_Trackball
 * @param event
 */
void ScreenController::handleMouseMoveEvent(QMouseEvent *event, float screen_width, float screen_height)
{
    // Reset drag if we're not dragging
    if (event->buttons() != Qt::LeftButton || event->modifiers() != Qt::ControlModifier) {
        is_dragging = false;
        prev_dragging_position = QVector3D();
        return;
    }

    // Normalize coordinates
    float x_ratio = event->position().x() / screen_width;
    float y_ratio = event->position().y() / screen_height;

    QVector3D mouse_pos = QVector3D(
        (1. - x_ratio) * 1. + x_ratio * -1.,    // Inverted to account for rotating a camera instead of the object
        (1. - y_ratio) * 1. + y_ratio * -1.,    // Inverted to account for rotating a camera instead of the object
        0.0
    );

    // Project onto sphere
    float sqr_z = 1.0f - QVector3D::dotProduct(mouse_pos, mouse_pos);
    if (sqr_z > 0) {
        mouse_pos.setZ(std::sqrt(sqr_z));
    } else {
        mouse_pos = 0.5f * mouse_pos.normalized();
    }

    // Reset if we are starting a drag
    QVector3D new_pos = mouse_pos.normalized();
    QVector3D last_pos = prev_dragging_position;
    QVector3D axis = QVector3D::crossProduct(last_pos, new_pos);
    if (!is_dragging || axis.length() == 0.0f) {
        is_dragging = true;
        prev_dragging_position = new_pos;
        return;
    }

    // Calculate angle and apply rotation.
    float angle = (180.f / M_PI) * std::acos(QVector3D::dotProduct(last_pos, new_pos));
    rotation = QQuaternion::fromAxisAndAngle(axis, angle) * rotation;
    prev_dragging_position = new_pos;

    updateTransformations();
    emit transformationChanged();
}

/**
 * @brief ScreenController::handleWheelEvent Zoom into the view if applicable
 * @param event
 */
void ScreenController::handleWheelEvent(QWheelEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier) {
        float phi = event->angleDelta().y() / 200.0f;
        translation.setZ(std::min(translation.z() + phi, 3.5f));
        updateTransformations();
        emit transformationChanged();;
    }
}
