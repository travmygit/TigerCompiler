#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* checked_malloc(size_t size)
{
	void* p = malloc(size);
	if (!p)
	{
		fprintf(stderr, "Ran out of memory!");
		exit(1);
	}
	return p;
}

string String(const char* src)
{
	string p = checked_malloc(strlen(src) + 1);
	strcpy(p, src);
	return p;
}

U_boolList U_BoolList(bool head, U_boolList tail)
{
	U_boolList list = checked_malloc(sizeof(*list));
	list->head = head;
	list->tail = tail;
	return list;
}
