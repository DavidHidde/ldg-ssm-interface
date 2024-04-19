#ifndef TREE_FUNCTIONS_H
#define TREE_FUNCTIONS_H

#include "drawing/model/tree_draw_properties.h"

size_t getParentIndex(size_t height, size_t index, TreeDrawProperties *tree_properties);

std::array<int, 4> getChildrenIndices(size_t height, size_t index, TreeDrawProperties *tree_properties);

#endif // TREE_FUNCTIONS_H
