#if 0

#include <stdio.h>
#include "util.h"
#include "slp.h"
#include "prog1.h"

int main(int argc, char** argv)
{
	printf("Welcome to tiger compiler!\n\n");

	// chap1
	A_stm stm = prog();
	printf("---chap1---\n");
	printf("maxargs:\n%d\n", maxargs(stm));
	printf("interp:\n");
	interp(stm);

	string nodes1[9] = { "t", "s", "p", "i", "p", "f", "b", "s", "t" };
	int bindings1[9] = {  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7 ,  8 ,  9  };
	string nodes2[9] = { "a", "b", "c", "d", "e", "f", "g", "h", "i" };
	int bindings2[9] = {  9 ,  8 ,  7 ,  6 ,  5 ,  4 ,  3 ,  2 ,  1  };
	T_tree t1 = NULL;
	for (int i = 0; i < 9; ++i)
	{
		t1 = insertTree(nodes1[i], &bindings1[i], t1);
		printf("insert %s -> size: %d  depth: %d\n", nodes1[i], i + 1, depthTree(t1));
	}
	printf("member b in tree1: %d\n", memberTree("b", t1));
	printf("member c in tree1: %d\n", memberTree("c", t1));
	printf("lookup b in tree1: %d\n", *(int*)lookupTree("b", t1));
	T_tree t2 = NULL;
	for (int i = 0; i < 9; ++i)
	{
		t2 = insertTree(nodes2[i], &bindings2[i], t2);
		printf("insert %s -> size: %d  depth: %d\n", nodes2[i], i + 1, depthTree(t2));
	}
	printf("lookup b in tree2: %d\n", *(int*)lookupTree("b", t2));

	return 0;
}

#endif