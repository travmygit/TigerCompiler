#pragma once

#include <assert.h>

typedef char* string;
typedef char  bool;

#define TRUE  1
#define FALSE 0

void* checked_malloc(size_t size);
string String(const char* src);

typedef struct U_boolList_* U_boolList;
struct U_boolList_
{
	bool head;
	U_boolList tail;
};
U_boolList U_BoolList(bool head, U_boolList tail);
