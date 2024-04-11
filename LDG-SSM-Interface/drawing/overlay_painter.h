#ifndef OVERLAYPAINTER_H
#define OVERLAYPAINTER_H

#include <QPainter>

#include "util/tree_draw_properties.h"

/**
 * @brief The OverlayPainter class Painter for drawing the grid overlay.
 */
class OverlayPainter : public QPainter
{
    TreeDrawProperties *draw_properties;

public:
    OverlayPainter(QPaintDevice *device, TreeDrawProperties *draw_properties);

    void drawOverlay();
};

#endif // OVERLAYPAINTER_H
