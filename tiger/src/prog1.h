#pragma once

#include "slp.h"
#include "util.h"

A_stm prog(void);

// chap1: tells the maximum number of arguments of any print statement within any subexpression of a given statement.
int maxargs(A_stm stm);

// chap1: ¡°interprets¡± a program in this language.
void interp(A_stm stm);

// chap1: exercises 1.1
#define OPTIMIZE_TREE_DISABLED 0

#if OPTIMIZE_TREE_DISABLED

typedef struct tree* T_tree;
struct tree
{
	T_tree left;
	T_tree right;
	string key;
	void* binding;
};
T_tree Tree(T_tree left, T_tree right, string key, void* binding);
T_tree insertTree(string key, void* binding, T_tree t);

#else // OPTIMIZE_TREE_DISABLED

// balanced search tree: left-leaning red black tree (LLRB).
#define RED   1
#define BLACK 0

typedef struct llrb* T_tree;
struct llrb
{
	T_tree left;
	T_tree right;
	string key;
	void* binding;
	bool color;
};
T_tree Tree(T_tree left, T_tree right, string key, void* binding, bool color);
T_tree insertTree(string key, void* binding, T_tree t);

#endif // OPTIMIZE_TREE_DISABLED

bool memberTree(string key, T_tree t);
void* lookupTree(string key, T_tree t);
int depthTree(T_tree t);