#include "prog1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

A_stm prog(void)
{
	return
		A_CompoundStm(A_AssignStm("a",
			A_OpExp(A_NumExp(5), A_plus, A_NumExp(3))),
			A_CompoundStm(A_AssignStm("b",
				A_EseqExp(A_PrintStm(A_PairExpList(A_IdExp("a"),
					A_LastExpList(A_OpExp(A_IdExp("a"), A_minus,
						A_NumExp(1))))),
					A_OpExp(A_NumExp(10), A_times, A_IdExp("a")))),
				A_PrintStm(A_LastExpList(A_IdExp("b")))));
}

//////////////////////////////////////////////////////////////////////////
// maxargs
//////////////////////////////////////////////////////////////////////////

int maxargsEseq(A_exp exp)
{
	if (exp->kind == A_eseqExp)
	{
		return max(maxargs(exp->u.eseq.stm), maxargsEseq(exp->u.eseq.exp));
	}
	else
	{
		return 0;
	}
}

int maxargs(A_stm stm)
{
	if (stm->kind == A_compoundStm)
	{
		return max(maxargs(stm->u.compound.stm1), maxargs(stm->u.compound.stm2));
	}
	else if (stm->kind == A_assignStm)
	{
		return maxargsEseq(stm->u.assign.exp);
	}
	else if (stm->kind == A_printStm)
	{
		int num = 0;
		int numExp = 0;
		A_expList exps = stm->u.print.exps;
		assert(exps != NULL);
		while (exps)
		{
			num++;
			if (exps->kind == A_pairExpList)
			{
				numExp = max(numExp, maxargsEseq(exps->u.pair.head));
				exps = exps->u.pair.tail;
			}
			else
			{
				assert(exps->kind == A_lastExpList);
				numExp = max(numExp, maxargsEseq(exps->u.last));
				exps = NULL;
			}
		}
		return max(num, numExp);
	}
	else
	{
		assert(0 && "unexpected branch!");
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////
// interp
//////////////////////////////////////////////////////////////////////////

typedef struct table* Table_;
struct table
{
	string id;
	int value;
	Table_ tail;
};
Table_ Table(string id, int value, Table_ tail)
{
	Table_ t = checked_malloc(sizeof(*t));
	t->id = id;
	t->value = value;
	t->tail = tail;
	return t;
}

typedef struct table2 IntAndTable_;
struct table2
{
	int i;
	Table_ t;
};
IntAndTable_ IntAndTable(int i, Table_ t)
{
	IntAndTable_ it;
	it.i = i;
	it.t = t;
	return it;
}

// Forward declaration
Table_ update(Table_ t, string id, int value);
int lookup(Table_ t, string key);
Table_ interpStm(A_stm stm, Table_ t);
IntAndTable_ interpExp(A_exp e, Table_ t);

Table_ update(Table_ t, string id, int value)
{
	return Table(id, value, t);
}

int lookup(Table_ t, string key)
{
	while (t)
	{
		if (strcmp(t->id, key) == 0)
		{
			return t->value;
		}
		t = t->tail;
	}
	assert(0 && "unexpected execution flow!");
	return 0;
}

Table_ interpStm(A_stm stm, Table_ t)
{
	if (stm->kind == A_compoundStm)
	{
		return interpStm(stm->u.compound.stm2, interpStm(stm->u.compound.stm1, t));
	}
	else if (stm->kind == A_assignStm)
	{
		IntAndTable_ nt = interpExp(stm->u.assign.exp, t);
		return update(nt.t, stm->u.assign.id, nt.i);
	}
	else if (stm->kind == A_printStm)
	{
		A_expList exps = stm->u.print.exps;
		bool first = TRUE;
		assert(exps != NULL);
		while (exps)
		{
			if (exps->kind == A_pairExpList)
			{
				IntAndTable_ nt = interpExp(exps->u.pair.head, t);
				t = nt.t;
				exps = exps->u.pair.tail;

				if (first)
				{
					first = FALSE;
					printf("%d", nt.i);
				}
				else
				{
					printf(" %d", nt.i);
				}
			}
			else
			{
				assert(exps->kind == A_lastExpList);
				IntAndTable_ nt = interpExp(exps->u.last, t);
				t = nt.t;
				exps = NULL;

				if (first)
				{
					first = FALSE;
					printf("%d\n", nt.i);
				}
				else
				{
					printf(" %d\n", nt.i);
				}
			}
		}
		return t;
	}
	else
	{
		assert(0 && "unexpected branch!");
		return NULL;
	}
}

IntAndTable_ interpExp(A_exp e, Table_ t)
{
	if (e->kind == A_idExp)
	{
		return IntAndTable(lookup(t, e->u.id), t);
	}
	else if (e->kind == A_numExp)
	{
		return IntAndTable(e->u.num, t);
	}
	else if (e->kind == A_opExp)
	{
		IntAndTable_ nt1 = interpExp(e->u.op.left, t);
		IntAndTable_ nt2 = interpExp(e->u.op.right, nt1.t);
		if (e->u.op.oper == A_plus)
		{
			nt2.i = nt1.i + nt2.i;
		}
		else if (e->u.op.oper == A_minus)
		{
			nt2.i = nt1.i - nt2.i;
		}
		else if (e->u.op.oper == A_times)
		{
			nt2.i = nt1.i * nt2.i;
		}
		else if (e->u.op.oper == A_div)
		{
			nt2.i = nt1.i / nt2.i;
		}
		else
		{
			assert(0 && "unexpected branch!");
		}
		return nt2;
	}
	else if (e->kind == A_eseqExp)
	{
		return interpExp(e->u.eseq.exp, interpStm(e->u.eseq.stm, t));
	}
	else
	{
		assert(0 && "unexpected branch!");
		return IntAndTable(0, NULL);
	}
}

void interp(A_stm stm)
{
	interpStm(stm, NULL);
}
