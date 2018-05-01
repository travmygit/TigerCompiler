#include "persistent_binary_tree.h"
#include <string.h>

T_tree Tree(T_tree l, string k, T_tree r) {
    T_tree t = checked_malloc(sizeof(*t));
    t->left = l;
    t->key = k;
    t->right = r;
    return t;
}

T_tree insert(string key, T_tree t) {
    if(key == NULL) return t;
    if(t == NULL)
        return Tree(NULL, key, NULL);
    else if(strcmp(key, t->key) < 0)
        return Tree(insert(key, t->left), t->key, t->right);
    else if(strcmp(key, t->key) > 0)
        return Tree(t->left, t->key, insert(key, t->right));
    else
        return Tree(t->left, t->key, t->right);
}

bool member(string key, T_tree t) {
    if(key == NULL || t == NULL)
        return FALSE;
	if(strcmp(key, t->key) < 0)
        return member(key, t->left);
    else if(strcmp(key, t->key) > 0)
        return member(key, t->right);
    else
        return TRUE;
}
