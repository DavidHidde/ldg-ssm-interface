#ifndef OVERLAYPAINTER_H
#define OVERLAYPAINTER_H

#include <QPainter>

#include <drawing/model/tree_draw_properties.h>
#include <drawing/model/window_draw_properties.h>

/**
 * @brief The OverlayPainter class Painter for drawing the grid overlay.
 */
class OverlayPainter : public QPainter
{
    TreeDrawProperties *tree_properties;
    WindowDrawProperties *window_properties;

public:
    OverlayPainter(QPaintDevice *device, TreeDrawProperties *tree_properties, WindowDrawProperties *window_properties);

    void drawOverlay();
};

#endif // OVERLAYPAINTER_H
