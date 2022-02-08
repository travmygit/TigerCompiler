#include <malloc.h>
#include "semant.h"
#include "types.h"
#include "env.h"
#include "errormsg.h"
#include "util.h"

typedef void *Tr_exp;

typedef struct expty_ {
    Tr_exp exp;
    Ty_ty  ty;
} expty;

expty expTy(Tr_exp exp, Ty_ty ty) {
    expty e;
    e.exp = exp;
    e.ty = ty;
    return e;
}

expty transVar(S_table venv, S_table tenv, A_var v);
expty transExp(S_table venv, S_table tenv, A_exp e);
void  transDec(S_table venv, S_table tenv, A_dec d);
Ty_ty transTy (S_table tenv, A_ty t);

// inside flag (for loop, while loop)
static int inside = 0;

// Turn 'Ty_name' to actual type
static Ty_ty actual_ty(Ty_ty ty) {
    Ty_ty t = ty;
    if (t->kind == Ty_name) {
        return actual_ty(ty->u.name.ty);
    }
    return t;
}

// Compare two actual type
// Ty_nil == Ty_record
// Ty_record or Ty_array: compare the reference
static bool actual_eq(Ty_ty source, Ty_ty target) {
    Ty_ty t1 = actual_ty(source);
    Ty_ty t2 = actual_ty(target);
    return ((t1->kind == Ty_record || t1->kind == Ty_array) && t1 == t2) ||
           (t1->kind == Ty_record && t2->kind == Ty_nil) ||
           (t1->kind == Ty_nil && t2->kind == Ty_record) ||
           (t1->kind != Ty_record && t1->kind != Ty_array && t1->kind == t2->kind);
}

void SEM_transProg(A_exp exp) {
    S_table venv = E_base_venv(), tenv = E_base_tenv();
    transExp(venv, tenv, exp);
}

// desc: translate variable
// return: actual type
expty transVar(S_table venv, S_table tenv, A_var v) {
    switch(v->kind) {
        case A_simpleVar: {
            E_enventry x = S_look(venv, v->u.simple);
            if(x && x->kind == E_varEntry) {
                return expTy(NULL, actual_ty(x->u.var.ty));
            } else {
                EM_error(v->pos, "simple var expression: undefined variable %s", S_name(v->u.simple));
                return expTy(NULL, Ty_Int());
            }
        }
        case A_fieldVar: {
            expty var = transVar(venv, tenv, v->u.field.var);
            if(var.ty->kind != Ty_record) {
                EM_error(v->u.field.var->pos, "field var expression: not a record type variable");
                return expTy(NULL, Ty_Int());
            } else {
                Ty_fieldList fl;
                for(fl = var.ty->u.record; fl; fl = fl->tail) {
                    if(fl->head->name == v->u.field.sym) {
                        return expTy(NULL, actual_ty(fl->head->ty));
                    }
                }
                EM_error(v->u.field.var->pos, "field var expression: no such field <%s> in the record", S_name(v->u.field.sym));
                return expTy(NULL, Ty_Int());
            }
        }
        case A_subscriptVar: {
            expty var = transVar(venv, tenv, v->u.subscript.var);
            if(var.ty->kind != Ty_array) {
                EM_error(v->u.subscript.var->pos, "subscript var expression: not an array type variable");
                return expTy(NULL, Ty_Int());
            } else {
                expty index = transExp(venv, tenv, v->u.subscript.exp);
                if(index.ty->kind != Ty_int) {
                    EM_error(v->u.subscript.exp->pos, "subscript var expression: integer required in array index");
                    return expTy(NULL, Ty_Int());
                }
                return expTy(NULL, actual_ty(var.ty->u.array));
            }
        }
    }
    assert(0); // wrong kind
}

// desc: translate expression
// return: actual type
expty transExp(S_table venv, S_table tenv, A_exp e) {
    switch(e->kind) {
        case A_varExp: return transVar(venv, tenv, e->u.var);
        case A_nilExp: return expTy(NULL, Ty_Nil());
        case A_intExp: return expTy(NULL, Ty_Int());
        case A_stringExp: return expTy(NULL, Ty_String());
        case A_callExp: {
            E_enventry fun_entry = S_look(venv, e->u.call.func);
            if(!fun_entry || (fun_entry->kind != E_funEntry)) {
                EM_error(e->pos, "call expression: undefined type %s", S_name(e->u.call.func));
                return expTy(NULL, Ty_Int());
            } else {
                A_expList el;
                Ty_tyList tl;
                for(el = e->u.call.args, tl = fun_entry->u.fun.formals; el && tl; el = el->tail, tl = tl->tail) {
                    expty exp = transExp(venv, tenv, el->head);
                    Ty_ty actual = actual_ty(tl->head);
                    if(!actual_eq(tl->head, exp.ty)) {
                        EM_error(el->head->pos, "call expression: argument type dosen't match the paramater");
                        return expTy(NULL, Ty_Int());
                    }
                }
                if(el) {
                    EM_error(el->head->pos, "call expression: too many arguments");
                    return expTy(NULL, Ty_Int());
                }
                if(tl) {
                    EM_error(e->pos, "call expression: not enough arguments");
                    return expTy(NULL, Ty_Int());
                }
                return expTy(NULL, actual_ty(fun_entry->u.fun.results));
            }
        } /* callexp */
        case A_opExp: {
            expty left  = transExp(venv, tenv, e->u.op.left);
            expty right = transExp(venv, tenv, e->u.op.right);
            switch(e->u.op.oper) {
                case A_plusOp:
                case A_minusOp:
                case A_timesOp:
                case A_divideOp: {
                    // operand must be integer, return value must be integer
                    if(left.ty->kind != Ty_int)
                        EM_error(e->u.op.left->pos, "binary operation: integer required");
                    if(right.ty->kind != Ty_int)
                        EM_error(e->u.op.right->pos, "binary operation: integer required");
                    return expTy(NULL, Ty_Int());
                }
                case A_eqOp:
                case A_neqOp: {
                    // can be applied to integers, strings, and two arrays or records
                    // be careful about records, because nil is also record-type
                    if(!actual_eq(left.ty, right.ty)) {
                        EM_error(e->pos, "operators compare: different type for compare");
                    }
                    return expTy(NULL, Ty_Int());
                }
                case A_ltOp:
                case A_leOp:
                case A_gtOp:
                case A_geOp: {
                    if(left.ty->kind != Ty_int || right.ty->kind != Ty_int) {
                        EM_error(e->pos, "binary compare: integer required");
                    }
                    return expTy(NULL, Ty_Int());
                }
            } /* switch */
        } /* A_opExp */
        case A_recordExp: {
            Ty_ty record_typ = S_look(tenv, e->u.record.typ);
            if(!record_typ) {
                EM_error(e->pos, "record expression: undefined type");
                return expTy(NULL, Ty_Record(NULL));
            }
            Ty_ty actual = actual_ty(record_typ);
            if(actual->kind != Ty_record) {
                EM_error(e->pos, "record expression: <%s> is not a record type", S_name(e->u.record.typ));
                return expTy(NULL, Ty_Record(NULL));
            }
            Ty_fieldList ty_fl;
            A_efieldList fl;
            for(fl = e->u.record.fields, ty_fl = actual->u.record; fl && ty_fl; fl = fl->tail, ty_fl = ty_fl->tail) {
                if(fl->head->name != ty_fl->head->name) {
                    EM_error(e->pos, "record expression: <%s> not a valid field name", S_name(fl->head->name));
                    return expTy(NULL, Ty_Record(NULL));
                }
                expty exp = transExp(venv, tenv, fl->head->exp);
                if(!actual_eq(exp.ty, ty_fl->head->ty)) {
                    EM_error(e->pos, "record expression: both field types dismatch");
                    return expTy(NULL, Ty_Record(NULL));
                }
            }
            return expTy(NULL, actual);
        }
        case A_seqExp: {
            expty exp = expTy(NULL, Ty_Void());
            A_expList el;
            for(el = e->u.seq; el; el = el->tail) {
                exp = transExp(venv, tenv, el->head);
            }
            return exp;
        }
        case A_assignExp: {
            expty var = transVar(venv, tenv, e->u.assign.var);
            expty exp = transExp(venv, tenv, e->u.assign.exp);
            if(!actual_eq(var.ty, exp.ty)) {
                EM_error(e->pos, "assign expression: dismatch type between variable and expression");
            }
            return expTy(NULL, Ty_Void());
        }
        case A_ifExp: {
            expty test = transExp(venv, tenv, e->u.iff.test);
            if(test.ty->kind != Ty_int) {
                EM_error(e->pos, "condition expression: test section must be integer");
                return expTy(NULL, Ty_Void());
            }
            expty then = transExp(venv, tenv, e->u.iff.then);
            if(e->u.iff.elsee) {
                expty elsee = transExp(venv, tenv, e->u.iff.elsee);
                if(!actual_eq(then.ty, elsee.ty)) {
                    EM_error(e->pos, "condition expression: then-else section must be the same type");
                    return expTy(NULL, Ty_Void());
                }
                return expTy(NULL, then.ty);
            } else {
                if(then.ty->kind != Ty_void) {
                    EM_error(e->pos, "condition expression: then section must be void");
                }
                return expTy(NULL, Ty_Void());
            }
        }
        case A_whileExp: {
            expty test = transExp(venv, tenv, e->u.whilee.test);
            if(test.ty->kind != Ty_int) {
                EM_error(e->u.whilee.test->pos, "while loop: test section must produce integer");
                return expTy(NULL, Ty_Void());
            }
            inside++; // inside loop
            expty body = transExp(venv, tenv, e->u.whilee.body);
            inside--; // outside
            if(body.ty->kind != Ty_void) {
                EM_error(e->u.whilee.body->pos, "while loop: body section must produce no value");
                return expTy(NULL, Ty_Void());
            }
            return expTy(NULL, Ty_Void());
        }
        case A_forExp: {
            expty lo = transExp(venv, tenv, e->u.forr.lo);
            expty hi = transExp(venv, tenv, e->u.forr.hi);
            if(lo.ty->kind != Ty_int) {
                EM_error(e->u.forr.lo->pos, "for loop: lower part must be integer");
                return expTy(NULL, Ty_Void());
            }
            if(hi.ty->kind != Ty_int) {
                EM_error(e->u.forr.hi->pos, "for loop: higer part must be integer");
                return expTy(NULL, Ty_Void());
            }
            S_beginScope(venv);
            S_enter(venv, e->u.forr.var, Ty_Int());
            inside++; // inside loop
            expty body = transExp(venv, tenv, e->u.forr.body);
            inside--; // outside
            if(body.ty->kind != Ty_void) {
                EM_error(e->u.forr.body->pos, "for loop: body part must be void");
                return expTy(NULL, Ty_Void());
            }
            S_endScope(venv);
            return expTy(NULL, Ty_Void());
        }
        case A_breakExp: {
            if(!inside) {
                EM_error(e->pos, "break expression: break expression outside loop");
            }
            return expTy(NULL, Ty_Void());
        }
        case A_letExp: {
            expty exp;
            A_decList d;
            S_beginScope(venv);
            S_beginScope(tenv);
            for(d = e->u.let.decs; d; d = d->tail) {
                transDec(venv, tenv, d->head);
            }
            exp = transExp(venv, tenv, e->u.let.body);
            S_endScope(tenv);
            S_endScope(venv);
            return exp;
        }
        case A_arrayExp: {
            Ty_ty array_typ = S_look(tenv, e->u.array.typ);
            if(!array_typ) {
                EM_error(e->pos, "array expression: undefined type %s", S_name(e->u.array.typ));
                return expTy(NULL, Ty_Array(NULL));
            }
            Ty_ty actual = actual_ty(array_typ);
            if(actual->kind != Ty_array) {
                EM_error(e->pos, "array expression: array type required but given another %s", S_name(e->u.array.typ));
                return expTy(NULL, Ty_Array(NULL));
            }
            expty size_typ = transExp(venv, tenv, e->u.array.size);
            if(size_typ.ty->kind != Ty_int) {
                EM_error(e->u.array.size->pos, "array expression: integer required with array size");
                return expTy(NULL, Ty_Array(NULL));
            }
            expty init_typ = transExp(venv, tenv, e->u.array.init);
            if(!actual_eq(init_typ.ty, actual->u.array)) {
                EM_error(e->u.array.init->pos, "array expression: initialize type does not match with given type");
                return expTy(NULL, Ty_Array(NULL));
            }
            return expTy(NULL, actual);
        }
    }
    assert(0); // should have returned from some clause of the switch
}

struct VisitedItem {
    Ty_ty ty;
    int legal;
    struct VisitedItem *next;
};
struct VisitedItem *visitedItemHead = NULL;
struct VisitedItem *getVisited(Ty_ty ty) {
    struct VisitedItem *tmp;
    for (tmp = visitedItemHead; tmp; tmp = tmp->next) {
        if (tmp->ty == ty) {
            return tmp;
        }
    }
    return NULL;
}
struct VisitedItem * addVisitedItem(Ty_ty ty, int legal) {
    struct VisitedItem *new = (struct VisitedItem *)malloc(sizeof *new);
    new->ty = ty;
    new->legal = legal;
    new->next = visitedItemHead;
    visitedItemHead = new;
}
void freeVisitedItem(struct VisitedItem *item) {
    if (item == NULL) {
        return;
    }
    freeVisitedItem(item->next);
    free(item);
}
int isLegalType(Ty_ty ty) {
    if (ty->kind != Ty_name) {
        return 1;
    }
    struct VisitedItem *i = getVisited(ty);
    if (i != NULL) {
        return i->legal;
    }
    struct VisitedItem *new = addVisitedItem(ty, 0);
    new->legal = isLegalType(ty->u.name.ty);
    return new->legal;
}
int checkRecursiveType(S_table tenv, A_nametyList nl) {
    for (; nl; nl = nl->tail) {
        Ty_ty t = S_look(tenv, nl->head->name);
        if (t->kind == Ty_name && !isLegalType(t)) {
            freeVisitedItem(visitedItemHead);
            visitedItemHead = NULL;
            return 0;
        }
    }
    freeVisitedItem(visitedItemHead);
    visitedItemHead = NULL;
    return 1;
}

void  transDec(S_table venv, S_table tenv, A_dec d) {
    switch(d->kind) {
        case A_varDec: {
            Ty_ty dec_ty = NULL; // declare type maybe NULL
            if(d->u.var.typ != NULL) {
                dec_ty = S_look(tenv, d->u.var.typ);
                if(!dec_ty) {
                    EM_error(d->pos, "variable declare: undefined type %s", S_name(d->u.var.typ));
                    return;
                }
            }
            expty init_exp = transExp(venv, tenv, d->u.var.init);
            // check declare type and initialize expression equal
            if(dec_ty != NULL) {
                if(!actual_eq(dec_ty, init_exp.ty)) {
                    EM_error(d->pos, "variable declare: dismatch type between declare and initialze");
                    return;
                }
            } else {
                if(init_exp.ty->kind == Ty_nil) {
                    EM_error(d->pos, "variable declare: illegal nil type: nil must be assign to a explictly record type");
                    return;
                }
            }
            S_enter(venv, d->u.var.var, E_VarEntry(init_exp.ty));
            return;
        }
        case A_typeDec: {
            A_nametyList type_list;
            bool cycle_decl = TRUE;

            int index = 0;
            void *typenames[10]; // store typenames in list, check for redeclaration

            // example: type list = { first: int, rest: list }
            // 1: add header (type list =) into environment e1
            for(type_list = d->u.type; type_list; type_list = type_list->tail) {
                S_enter(tenv, type_list->head->name, Ty_Name(type_list->head->name, NULL));
                for(int i = 0; i < index; i++) {
                    if(typenames[i] == (void *)type_list->head->name) {
                        EM_error(type_list->head->ty->pos, "type declare: redeclaration type <%s>, there are two types with the same name in the same (consecutive) batch of mutually recursive types.", S_name(type_list->head->name));
                    }
                }
                typenames[index++] = (void *)type_list->head->name;
            }
            // 2: translate body ({ first: int, rest: list }) to fill the place-holder in the e1
            for(type_list = d->u.type; type_list; type_list = type_list->tail) {
                Ty_ty t = transTy(tenv, type_list->head->ty);
                Ty_ty name_type = S_look(tenv, type_list->head->name);
                name_type->u.name.ty = t;
            }
            int result = checkRecursiveType(tenv, d->u.type);
            if(result == 0) {
                EM_error(d->pos, "illegal type cycle");
            }
            return;
        }
        case A_functionDec: {
            A_fundecList fun_list;

            int index = 0;
            void *typenames[10]; // store typenames in list, check for redeclaration

            // example: function treeLeaves(t: tree): int = treelistLeaves(t.children)
            // 1: add header (function treeLeaves(t: tree): int =)
            for(fun_list = d->u.function; fun_list; fun_list = fun_list->tail) {
                // i need elements of E_FunEntry, because its value environment
                A_fieldList fl;
                Ty_ty ty;

                Ty_tyList head = NULL, tail = NULL;
                Ty_ty r;

                // return type
                if(fun_list->head->result) {
                    r = S_look(tenv, fun_list->head->result);
                    if(!r) {
                        EM_error(fun_list->head->pos, "function declare: undefined return type %s", S_name(fun_list->head->result));
                        return;
                    }
                } else {
                    r = Ty_Void();
                }
                // parameters
                for(fl = fun_list->head->params; fl; fl = fl->tail) {
                    ty = S_look(tenv, fl->head->typ);
                    if(!ty) {
                        EM_error(fl->head->pos, "function declare: undefined parameter type %s", S_name(fl->head->typ));
                        return;
                    }
                    if(head) {
                        tail->tail = Ty_TyList(ty, NULL);
                        tail = tail->tail;
                    } else {
                        head = Ty_TyList(ty, NULL);
                        tail = head;
                    }
                }
                S_enter(venv, fun_list->head->name, E_FunEntry(head, r));

                for(int i = 0; i < index; i++) {
                    if(typenames[i] == (void *)fun_list->head->name) {
                        EM_error(fun_list->head->pos, "type declare: redeclaration type <%s>, there are two types with the same name in the same (consecutive) batch of mutually recursive types.", S_name(fun_list->head->name));
                    }
                }
                typenames[index++] = (void *)fun_list->head->name;
            }
            // 2: translate body (treelistLeaves(t.children))
            for(fun_list = d->u.function; fun_list; fun_list = fun_list->tail) {
                E_enventry fun_entry = S_look(venv, fun_list->head->name);
                S_beginScope(venv);
                // add parameters into environment
                A_fieldList fl;
                Ty_tyList tl = fun_entry->u.fun.formals;

                for(fl = fun_list->head->params; fl; fl = fl->tail, tl = tl->tail) {
                    S_enter(venv, fl->head->name, E_VarEntry(tl->head));
                }
                // translate body
                expty exp = transExp(venv, tenv, fun_list->head->body);
                // compare return type and body type
                if(!actual_eq(fun_entry->u.fun.results, exp.ty)) {
                    EM_error(d->pos, "function declare: body type and return type with <%s>", S_name(fun_list->head->name));
                    S_endScope(venv);
                    return;
                }
                S_endScope(venv);
            }
            return;
        }
    }
}

Ty_ty transTy (S_table tenv, A_ty t) {
    switch(t->kind) {
        case A_nameTy: {
            Ty_ty ty = S_look(tenv, t->u.name);
            if(!ty) {
                EM_error(t->pos, "translate name type: undefined type %s", S_name(t->u.name));
                return Ty_Void();
            }
            return ty;
        }
        case A_recordTy: {
            A_fieldList fl;
            Ty_field ty_f;
            Ty_fieldList ty_fl_head = NULL, ty_fl_tail = NULL;
            Ty_ty ty;
            for(fl = t->u.record; fl; fl = fl->tail) {
                ty = S_look(tenv, fl->head->typ);
                if(!ty) {
                    EM_error(fl->head->pos, "translate record type: undefined type %s", S_name(fl->head->typ));
                    return Ty_Void();
                }
                ty_f = Ty_Field(fl->head->name, ty);
                if(ty_fl_head) {
                    ty_fl_tail->tail = Ty_FieldList(ty_f, NULL);
                    ty_fl_tail = ty_fl_tail->tail;
                } else {
                    ty_fl_head = Ty_FieldList(ty_f, NULL);
                    ty_fl_tail = ty_fl_head;
                }
            }
            return Ty_Record(ty_fl_head);
        }
        case A_arrayTy: {
            Ty_ty ty = S_look(tenv, t->u.array);
            if(!ty) {
                EM_error(t->pos, "translate array type: undefined type %s", S_name(t->u.array));
                return Ty_Void();
            }
            return Ty_Array(ty);
        }
    }
    assert(0); // should have returned from some clause of the switch
}
