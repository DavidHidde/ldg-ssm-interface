#ifndef BUILD_TREE_H
#define BUILD_TREE_H

#include "../widgets/ldg_tree_view_button.h"
#include "../ldg_ssm_interface.h"

LDGTreeViewButton *buildTree(size_t num_rows, size_t num_cols, LDGSSMInterface *parent);

#endif // BUILD_TREE_H
