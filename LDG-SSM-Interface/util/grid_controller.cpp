#include "grid_controller.h"

/**
 * @brief GridController::GridController
 * @param draw_properties
 */
GridController::GridController(TreeDrawProperties *draw_properties):
    draw_properties(draw_properties)
{
}

/**
 * @brief GridController::splitNode
 * @param height
 * @param index
 */
void GridController::splitNode(size_t height, size_t index)
{

}

/**
 * @brief GridController::mergeNode
 * @param height
 * @param index
 */
void GridController::mergeNode(size_t height, size_t index)
{

}

/**
 * @brief GridController::fromWorldSpaceToScreenSpace
 * @param point
 * @return
 */
QPoint GridController::fromWorldSpaceToScreenSpace(QPoint &point)
{
    return { 0, 0, };
}

/**
 * @brief GridController::handleMouseClick
 * @param event
 */
void GridController::handleMouseClick(QMouseEvent *event)
{

}

/**
 * @brief GridController::selectHeight
 * @param height
 */
void GridController::selectHeight(size_t height)
{
    if (height <= draw_properties->tree_max_height) {
        auto [num_rows, num_cols] = draw_properties->height_dims[height];
        draw_properties->draw_array.clear();

        for (size_t idx = 0; idx < num_rows * num_cols; ++idx)
            draw_properties->draw_array.push_back({ height, idx });

        emit gridChanged();
    }
}
