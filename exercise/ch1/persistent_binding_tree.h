#ifndef PERSISTENT_BINDING_TREE_H
#define PERSISTENT_BINDING_TREE_H

#include "util.h"

typedef struct binding *T_binding;
struct binding {
    string key;
    void *value;
};

T_binding Binding(string k, void *v);

typedef struct tree *T_tree;
struct tree {
    T_tree left;
    T_binding binding;
    T_tree right;
};

T_tree Tree(T_tree l, T_binding b, T_tree r);

T_tree insert(string key, void *binding, T_tree t);

void* lookup(string key, T_tree t);

#endif

