#include "square_grid_layout.h"

/**
 * @brief SquareGridLayout::setGeometry Set the geometry, making sure everything remains square.
 * @param rect
 */
void SquareGridLayout::setGeometry(const QRect &rect)
{
    auto max_dim = rect.height() > rect.width() ? rect.width() : rect.height();
    QRect new_rect(rect.x(), rect.y(), max_dim, max_dim);
    QGridLayout::setGeometry(new_rect);
}
