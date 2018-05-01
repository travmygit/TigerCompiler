#ifndef PERSISTENT_BINARY_TREE_H
#define PERSISTENT_BINARY_TREE_H

#include "util.h"

typedef struct tree *T_tree;
struct tree {
    T_tree left;
    string key;
    T_tree right;
};

T_tree Tree(T_tree l, string k, T_tree r);

T_tree insert(string key, T_tree t);

bool member(string key, T_tree t);

#endif
