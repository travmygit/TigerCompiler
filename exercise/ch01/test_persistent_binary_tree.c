#include <stdio.h>
#include <stdlib.h>
#include "persistent_binary_tree.h"

void print_tree(T_tree t) {
    if(t == NULL) return;
    printf("%s\n", t->key);
    print_tree(t->left);
    print_tree(t->right);
}

int main() {
    T_tree tree = NULL;
    int i;
    char *key = NULL;
    
    for(int i = 0; i < 10; i++) {
        key = (char *)malloc(sizeof(char)*2);
        key[0] = '0' + i;
        tree = insert(key, tree);
    }
    
    print_tree(tree);
    
    key = (char *)malloc(sizeof(char)*2);
    key[0] = 'a';
    
    if(member(key, tree)) printf("%s found in tree.\n", key);
    else printf("%s not found.\n", key);
    
    return 0;
}
