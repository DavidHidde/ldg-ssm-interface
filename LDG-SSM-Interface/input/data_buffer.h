#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H

#include "drawing/model/tree_draw_properties.h"
#include <QImage>
#include <QMap>
#include <QString>

QPair<QMap<QPair<size_t, size_t>, QPair<QImage, double>> *, TreeDrawProperties *> readInput(QString visualization_configuration_path);

#endif // DATA_BUFFER_H
