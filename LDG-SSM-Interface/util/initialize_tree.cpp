#include "initialize_tree.h"

/**
 * @brief buildTree Setup draw properties based on given dimnensions.
 * @param num_rows
 * @param num_cols
 * @return
 */
TreeDrawProperties *initializeTreeProperties(size_t num_rows, size_t num_cols)
{
    // Build tree dimensions.
    size_t height = 1;
    QList<std::pair<size_t, size_t>> height_dims{ { num_rows, num_cols } };
    while (num_rows > 1 || num_cols > 1) {
        num_rows = num_rows / 2 + num_rows % 2;
        num_cols = num_cols / 2 + num_cols % 2;
        height_dims.append({ num_rows, num_cols });
        ++height;
    }

    // Build draw properties. The origin point and sidelens are later recalculated based on window size.
    QPoint origin{ 0, 0 };
    QList<std::pair<size_t, size_t>> draw_array{ { height, 0 } }; // Contains root by default.
    QList<size_t> click_assignment(num_rows * num_cols, 0); // Only root visible so all clicks click the root.
    QList<size_t> height_lens(height + 1, 0);

    TreeDrawProperties *properties = new TreeDrawProperties{
        5,  // Default spacing
        origin,
        draw_array,
        click_assignment,
        height_lens,
        height,
        height_dims
    };

    return properties;
}
