#include <stdio.h>
#include <stdlib.h>
#include "persistent_binding_tree.h"

void print_tree(T_tree t) {
    if(t == NULL) return;
    printf("[%s](%s)\n", t->binding->key, t->binding->value);
    print_tree(t->left);
    print_tree(t->right);
}

int main() {
    T_tree tree = NULL;
    int i;
    
    char *key = NULL;
    char *value = NULL;
    
    for(int i = 0; i < 10; i++) {
        key = (char *)malloc(sizeof(char)*2);
        value = (char *)malloc(sizeof(int)*10);
        key[0] = '0' + i;
        srand(i);
        for(int j = 0; j < 11; j++)
            value[j] = '0' + (int)rand() % 10;
        tree = insert(key, value, tree);
    }
    
    print_tree(tree);
    
    if((value = lookup(key, tree)) != NULL) printf("[%s](%s) found in tree.\n", key, value);
    else printf("[%s] not found.\n", key);
    
    value = (char *)malloc(sizeof(int)*10);
    
    tree = insert(key, value, tree);
    for(int j = 0; j < 11; j++)
        value[j] = '0' + (int)rand() % 10;
    print_tree(tree);
    
    return 0;
}
