#ifndef INTERP_H
#define INTERP_H

#include "util.h"
#include "slp.h"

typedef struct table *Table_;
struct table {
	string id;
	int value;
	Table_ tail;
};

Table_ Table(string id, int value, struct table *tail);

Table_ interpStm(A_stm s, Table_ t);

struct IntAndTable {
	int i;
	Table_ t;
};

struct IntAndTable interExp(A_exp e, Table_ t);

int lookup(Table_ t, string key);

/* Interpret a program in this language */
void interp(A_stm stm);

#endif
