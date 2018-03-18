#include "interp.h"
#include <stdio.h>
#include <stdlib.h>

static int LOOK_UP_VALID = 1;

/* Represent a table, mapping identifiers to the integer values
   assigned to them, as a list of id x int pairs.
   The empty table is presented as NULL. */
Table_ Table(string id, int value, struct table *tail) {
    Table_ t = checked_malloc(sizeof(*t));
    t->id = id;
    t->value = value;
    t->tail = tail;
    return t;
}

/* Taking a table t1 as argument and producing the new table t2 that's just like
   t1 except that some identifiers map to different integers as a result of the
   statement. */
Table_ interpStm(A_stm s, Table_ t) {
    if(s->kind == A_compoundStm) {
        t = interpStm(s->u.compound.stm1, t);
        t = interpStm(s->u.compound.stm2, t);
        return t;
    } else if(s->kind == A_assignStm) {
        struct IntAndTable it = interExp(s->u.assign.exp, t);
        return Table(s->u.assign.id, it.i, it.t);
    } else if(s->kind == A_printStm) {
        struct IntAndTable it;
        A_expList list = s->u.print.exps;
        if(list->kind == A_lastExpList) {
            it = interExp(list->u.last, t);
            printf("%d\n", it.i);
            return it.t;
        } else {
            while(list->kind != A_lastExpList) {
                it = interExp(list->u.pair.head, t);
                printf("%d ", it.i);
                t = it.t;
                list = list->u.pair.tail;
            }
            it = interExp(list->u.last, t);
            printf("%d\n", it.i);
            return it.t;
        }
    } else {
        // syntax wrong
        assert(!"Syntax Error");
    }
}

/* The result of interpreting an expression e1 with table t1 is an integer value i
   and a new table t2. When interpreting an expression with two subexpressions
   (such as an OpExp), the table t2 resulting from the first subexpression can be
   used in processing the second subexpression. */
struct IntAndTable interExp(A_exp e, Table_ t) {
    if(e->kind == A_idExp) {
        int value = lookup(t, e->u.id);
        // validate
        assert(LOOK_UP_VALID);
        
        struct IntAndTable it;
        it.t = t;
        it.i = value;
        return it;
    } else if(e->kind == A_numExp) {
        struct IntAndTable it;
        it.t = t;
        it.i = e->u.num;
        return it;
    } else if(e->kind == A_opExp) {
        struct IntAndTable it;
        struct IntAndTable left = interExp(e->u.op.left, t);
        struct IntAndTable right = interExp(e->u.op.right, left.t);
        switch(e->u.op.oper) {
            case A_plus: it.i = left.i + right.i; break;
            case A_minus: it.i = left.i - right.i; break;
            case A_times: it.i = left.i * right.i; break;
            case A_div: it.i = left.i / right.i; break;
            default:
                assert(!"Unknown operator");
        }
        it.t = right.t;
        return it;
    } else if(e->kind == A_eseqExp) {
        struct IntAndTable it;
        t = interpStm(e->u.eseq.stm, t);
        it = interExp(e->u.eseq.exp, t);
        return it;
    } else {
        // syntax wrong
        assert(!"Syntax Error");
    }
}

/* Look up table for the valid key value. */
int lookup(Table_ t, string key) {
    if(t == NULL) {
        return LOOK_UP_VALID = 0;
    }
    if(key == NULL) {
        return LOOK_UP_VALID = 0;
    }
    
    Table_ next = t;
    
    while(next != NULL) {
        if(next->id == key) {
            LOOK_UP_VALID = 1;
            return next->value;
        }
        next = t->tail;
    }
    return LOOK_UP_VALID = 0;
}

void interp(A_stm stm) {
    interpStm(stm, NULL);
}
