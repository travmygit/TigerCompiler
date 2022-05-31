#pragma once

#include "slp.h"
#include "util.h"

A_stm prog(void);

// chap1: tells the maximum number of arguments of any print statement within any subexpression of a given statement.
int maxargs(A_stm stm);

// chap1: ¡°interprets¡± a program in this language.
void interp(A_stm stm);

// chap1: exercises 1.1
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
bool memberTree(string key, T_tree t);
void* lookupTree(string key, T_tree t);
int depthTree(T_tree t);