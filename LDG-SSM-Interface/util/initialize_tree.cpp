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

    // Build draw properties. The sidelens are later recalculated based on window size.
    // QList<std::pair<size_t, size_t>> draw_array{ { height - 1, 0 } }; // Contains root by default.
    QList<int> click_assignment(num_rows * num_cols, 0); // Only root visible so all clicks click the root.
    QList<double> height_lens(height, 0.);
    QList<std::pair<size_t, size_t>> draw_array{
        { height - 2, 0 },
        { height - 2, 1 },
        { height - 2, 2 },
        { height - 2, 3 },
        { height - 3, 5 }
    }; // Contains root by default.

    TreeDrawProperties *properties = new TreeDrawProperties{
        5, // Default spacing
        1., // Scaling factor
        draw_array,
        click_assignment,
        height_lens,
        --height,
        height_dims
    };

    return properties;
}
