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

//////////////////////////////////////////////////////////////////////////
// tree
//////////////////////////////////////////////////////////////////////////

#if OPTIMIZE_TREE_DISABLED

T_tree Tree(T_tree left, T_tree right, string key, void* binding)
{
	T_tree t = checked_malloc(sizeof(*t));
	t->left = left;
	t->right = right;
	t->key = key;
	t->binding = binding;
	return t;
}

T_tree insertTree(string key, void* binding, T_tree t)
{
	if (t == NULL)
	{
		return Tree(NULL, NULL, key, binding);
	}
	else if (strcmp(key, t->key) < 0)
	{
		return Tree(insertTree(key, binding, t->left), t->right, t->key, t->binding);
	}
	else if (strcmp(key, t->key) > 0)
	{
		return Tree(t->left, insertTree(key, binding, t->right), t->key, t->binding);
	}
	else
	{
		return Tree(t->left, t->right, key, binding);
	}
}

#else // OPTIMIZE_TREE_DISABLED

//////////////////////////////////////////////////////////////////////////
// LLRB
//////////////////////////////////////////////////////////////////////////

bool isRed(T_tree h)
{
	if (h == NULL)
	{
		return FALSE;
	}
	return h->color == RED;
}

// rotate a right-leaning to left-leaning
T_tree rotateLeft(T_tree h)
{
	assert(h);
	assert(isRed(h->right));
	T_tree x = h->right;
	h->right = x->left;
	x->left = h;
	x->color = h->color;
	h->color = RED;
	return x;
}

// rotate a left-leaning to right-leaning
T_tree rotateRight(T_tree h)
{
	assert(h);
	assert(isRed(h->left));
	T_tree x = h->left;
	h->left = x->right;
	x->right = h;
	x->color = h->color;
	h->color = RED;
	return x;
}

void flipColor(T_tree h)
{
	assert(h);
	assert(!isRed(h));
	assert(isRed(h->left));
	assert(isRed(h->right));
	h->color = RED;
	h->left->color = BLACK;
	h->right->color = BLACK;
}

T_tree insertNode(string key, void* binding, T_tree t)
{
	if (t == NULL)
	{
		return Tree(NULL, NULL, key, binding, RED);
	}

	T_tree h;
	int cmp = strcmp(key, t->key);
	if (cmp < 0)
	{
		h = Tree(insertNode(key, binding, t->left), t->right, t->key, t->binding, t->color);
	}
	else if (cmp > 0)
	{
		h = Tree(t->left, insertNode(key, binding, t->right), t->key, t->binding, t->color);
	}
	else
	{
		h = Tree(t->left, t->right, key, binding, t->color);
	}

	// fix-up any right-leaning links
	if (isRed(h->right) && !isRed(h->left))
	{
		h = rotateLeft(h);
	}
	if (isRed(h->left) && isRed(h->left->left))
	{
		h = rotateRight(h);
	}
	if (isRed(h->left) && isRed(h->right))
	{
		flipColor(h);
	}

	return h;
}

bool isBST_internal(T_tree t, string min, string max)
{
	if (t == NULL)
	{
		return TRUE;
	}
	if (min != NULL && strcmp(min, t->key) >= 0)
	{
		return FALSE;
	}
	if (max != NULL && strcmp(max, t->key) <= 0)
	{
		return FALSE;
	}
	return isBST_internal(t->left, min, t->key) && isBST_internal(t->right, t->key, max);
}

bool isBST(T_tree root)
{
	return isBST_internal(root, NULL, NULL);
}

bool is23_internal(T_tree t)
{
	if (t == NULL)
	{
		return TRUE;
	}
	if (isRed(t->right))
	{
		return FALSE;
	}
	if (isRed(t) && isRed(t->left))
	{
		return FALSE;
	}
	return is23_internal(t->left) && is23_internal(t->right);
}

bool is23(T_tree root)
{
	return is23_internal(root);
}

bool isBalanced_internal(T_tree t, int black)
{
	if (t == NULL)
	{
		return black == 0;
	}
	if (!isRed(t))
	{
		black--;
	}
	return isBalanced_internal(t->left, black) && isBalanced_internal(t->right, black);
}

bool isBalanced(T_tree root)
{
	int black = 0;
	T_tree t = root;
	while (t)
	{
		if (!isRed(t))
		{
			black++;
		}
		t = t->left;
	}
	return isBalanced_internal(root, black);
}

bool checkLLRB(T_tree root)
{
	if (!isBST(root))
	{
		printf("Not BST!");
	}
	if (!is23(root))
	{
		printf("Not 2-3 Tree!");
	}
	if (!isBalanced(root))
	{
		printf("Not Balanced Tree!");
	}
	return isBST(root) && is23(root) && isBalanced(root);
}

T_tree Tree(T_tree left, T_tree right, string key, void* binding, bool color)
{
	T_tree t = checked_malloc(sizeof(*t));
	t->left = left;
	t->right = right;
	t->key = key;
	t->binding = binding;
	t->color = color;
	return t;
}

T_tree insertTree(string key, void* binding, T_tree t)
{
	T_tree root = insertNode(key, binding, t);
	root->color = BLACK;
	assert(checkLLRB(root));
	return root;
}

#endif // OPTIMIZE_TREE_DISABLED

bool memberTree(string key, T_tree t)
{
	while (t)
	{
		int cmp = strcmp(key, t->key);
		if (cmp < 0)
		{
			t = t->left;
		}
		else if (cmp > 0)
		{
			t = t->right;
		}
		else
		{
			return TRUE;
		}
	}
	return FALSE;
}

void* lookupTree(string key, T_tree t)
{
	while (t)
	{
		int cmp = strcmp(key, t->key);
		if (cmp < 0)
		{
			t = t->left;
		}
		else if (cmp > 0)
		{
			t = t->right;
		}
		else
		{
			return t->binding;
		}
	}
	return NULL;
}

int depthTree(T_tree t)
{
	if (t == NULL)
	{
		return 0;
	}
	else
	{
		return max(depthTree(t->left), depthTree(t->right)) + 1;
	}
}