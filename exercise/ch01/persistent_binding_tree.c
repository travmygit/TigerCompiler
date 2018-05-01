#include "persistent_binding_tree.h"
#include <string.h>

T_binding Binding(string k, void *v) {
    T_binding b = checked_malloc(sizeof(*b));
    b->key = k;
    b->value = v;
    return b;
}

T_tree Tree(T_tree l, T_binding b, T_tree r) {
    T_tree t = checked_malloc(sizeof(*t));
    t->left = l;
    t->binding = b;
    t->right = r;
    return t;
}

T_tree insert(string key, void *binding, T_tree t) {
    if(key == NULL) return t;
    if(t == NULL)
        return Tree(NULL, Binding(key, binding), NULL);
    else if(strcmp(key, t->binding->key) < 0)
        return Tree((insert(key, binding, t->left)), t->binding, t->right);
    else if(strcmp(key, t->binding->key) > 0)
        return Tree(t->left, t->binding, (insert(key, binding, t->right)));
    else
        return Tree(t->left, Binding(key, binding), t->right);
}

void* lookup(string key, T_tree t) {
    if(t == NULL || key == NULL) return NULL;
    if(strcmp(key, t->binding->key) < 0)
        return lookup(key, t->left);
    else if(strcmp(key, t->binding->key) > 0)
        return lookup(key, t->right);
    else
        return t->binding->value;
}
