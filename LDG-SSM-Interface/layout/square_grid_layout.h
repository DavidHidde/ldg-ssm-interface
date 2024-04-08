#ifndef SQUARE_GRID_LAYOUT_H
#define SQUARE_GRID_LAYOUT_H

#include <QGridLayout>

/**
 * @brief The SquareGridLayout class Grid layout that makes it so that all grid items have a square aspect ratio
 */
class SquareGridLayout : public QGridLayout
{
public:
    void setGeometry(const QRect &rect) override;
};

#endif // SQUARE_GRID_LAYOUT_H
