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

static Ty_ty actual_ty(Ty_ty ty) {
    Ty_ty t = ty;
    while(t->kind == Ty_name) t = t->u.name.ty;
    return t;
}

void SEM_transProg(A_exp exp) {
    S_table venv = E_base_venv(), tenv = E_base_tenv();
    transExp(venv, tenv, exp);
}

expty transVar(S_table venv, S_table tenv, A_var v) {
    switch(v->kind) {
        case A_simpleVar: {
            E_enventry x = S_look(venv, v->u.simple);
            if(x && x->kind == E_varEntry)
                return expTy(NULL, actual_ty(x->u.var.ty));
            else {
                EM_error(v->pos, "undefined variable %s", S_name(v->u.simple));
                return expTy(NULL, Ty_Int());
            }
        }
        case A_fieldVar: {
            expty var = transVar(venv, tenv, v->u.field.var);
            if(var.ty->kind != Ty_record) {
                EM_error(v->u.field.var->pos, "variable must be a record type");
            } else {
                Ty_fieldList fl;

                for(fl = var.ty->u.record; fl; fl = fl->tail) {
                    if(fl->head->name == v->u.field.sym)
                        return expTy(NULL, actual_ty(fl->head->ty));
                }
                EM_error(v->pos, "no such field %s in the record fields", S_name(v->u.field.sym));
            }
            return expTy(NULL, Ty_Int());
        }
        case A_subscriptVar: {
            expty var = transVar(venv, tenv, v->u.subscript.var);
            if(var.ty->kind != Ty_array) {
                EM_error(v->u.subscript.var->pos, "variable must be an array type");
                return expTy(NULL, Ty_Int());
            } else {
                expty index = transExp(venv, tenv, v->u.subscript.exp);
                if(index.ty->kind != Ty_int)
                    EM_error(v->u.subscript.exp->pos, "integer required");
                return expTy(NULL, actual_ty(var.ty->u.array));
            }
        }
    }
}

expty transExp(S_table venv, S_table tenv, A_exp e) {
    switch(e->kind) {
        case A_varExp: return transVar(venv, tenv, e->u.var);
        case A_nilExp: return expTy(NULL, Ty_Nil());
        case A_intExp: return expTy(NULL, Ty_Int());
        case A_stringExp: return expTy(NULL, Ty_String());
        case A_callExp: {
            E_enventry fun_entry = S_look(venv, e->u.call.func);
            if(!fun_entry || fun_entry != E_FunEntry) {
                EM_error(e->pos, "undefined type %s", S_name(e->u.call.func));
                return expTy(NULL, Ty_Int());
            } else {
                A_expList el;
                Ty_tyList tl;
                for(el = e->u.call.args, tl = fun_entry->u.fun.formals; el && tl; el = el->tail, tl = tl->tail) {
                    expty exp = transExp(venv, tenv, el->head);
                    Ty_ty actual = actual_ty(tl->head);
                    if(actual->kind == Ty_array) {
                        if(exp.ty->kind != Ty_array || exp.ty != actual)
                            EM_error(el->head->pos, "function call: dismatch type");
                    } else if(actual->kind == Ty_record) {
                        if(exp.ty->kind != Ty_record || exp.ty != Ty_nil)
                            EM_error(el->head->pos, "function call: expect record type");
                        if(exp.ty->kind == Ty_record && exp.ty != actual)
                            EM_error(el->head->pos, "function call: different reference of record type");
                    } else if(actual->kind != exp.ty->kind) {
                        EM_error(el->head->pos, "function call: dismatch type");
                    }
                }
                if(el)
                    EM_error(el->head->pos, "too many arguments");
                if(tl)
                    EM_error(e->pos, "not enough arguments");
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
                        EM_error(e->u.op.left->pos, "integer required");
                    if(right.ty->kind != Ty_int)
                        EM_error(e->u.op.right->pos, "integer required");
                }
                case A_eqOp:
                case A_neqOp: {
                    // can be applied to integers, strings, and two arrays or records
                    // be careful about records, because nil is also record-type
                    if(left.ty->kind == Ty_int) {
                        if(right.ty->kind != Ty_int)
                            EM_error(e->u.op.right->pos, "integer required");
                    } else if(left.ty->kind == Ty_array) {
                        // 1: check type
                        if(right.ty->kind != Ty_array)
                            EM_error(e->u.op.right->pos, "array required");
                    } else if(left.ty->kind == Ty_record) {
                        // 1: check type
                        if(right.ty->kind != Ty_record || right.ty->kind != Ty_nil)
                            EM_error(e->u.op.right->pos, "record or nil required");
                    } else if(left.ty->kind == Ty_nil) {
                        // 1: check type
                        if(right.ty->kind != Ty_record)
                            EM_error(e->u.op.right->pos, "record required");
                    } else if(left.ty->kind == Ty_string) {
                        if(right.ty->kind != Ty_string)
                            EM_error(e->u.op.right->pos, "string required");
                    } else {
                        EM_error(e->u.op.left->pos, "unkonwn types to compare!");
                    }
                }
                case A_ltOp:
                case A_leOp:
                case A_gtOp:
                case A_geOp: {
                    if(left.ty->kind == Ty_int) {
                        if(right.ty->kind != Ty_int)
                            EM_error(e->u.op.right->pos, "integer required");
                    } else {
                        EM_error(e->u.op.left->pos, "wrong types to compare!");
                    }
                }
            } /* switch */
            return expTy(NULL, Ty_Int());
        } /* A_opExp */
        case A_recordExp: {
            
        }
        case A_seqExp: {

        }
        case A_assignExp: {

        }
        case A_ifExp: {

        }
        case A_whileExp: {

        }
        case A_forExp: {

        }
        case A_breakExp: {

        }
        case A_letExp: {
            expty exp;
            A_decList d;
            S_beginScope(venv);
            S_beginScope(tenv);
            for(d = e->u.let.decs; d; d = d->tail)
                transDec(venv, tenv, d->head);
            exp = transExp(venv, tenv, e->u.let.body);
            S_endScope(tenv);
            S_endScope(venv);
            return exp;
        }
        case A_arrayExp: {

        }
    }
    assert(0); /* should have returned from some clause of the switch */
}

void  transDec(S_table venv, S_table tenv, A_dec d) {
    switch(d->kind) {
        case A_varDec: {
            Ty_ty dec_ty = NULL; // maybe NULL
            if(d->u.var.typ != NULL) {
                dec_ty = S_look(tenv, d->u.var.typ);
                if(!dec_ty)
                    EM_error(d->pos, "undefined type %s", S_name(d->u.var.typ));
            }
            expty init_exp = transExp(venv, tenv, d->u.var.init);
            // check declare type and initialize expression equal
            // i have to remind you, transExp() return Ty_tys except Ty_name
            // but: the declare type may be Ty_name
            if(dec_ty != NULL) {
                Ty_ty actual_dec_ty = actual_ty(dec_ty);
                if(actual_dec_ty == Ty_array) {
                    if(init_exp.ty != Ty_array || init_exp.ty != actual_dec_ty)
                        EM_error(d->pos, "illegal array variable: expected type %s, but given another initialize type, maybe different reference", S_name(d->u.var.typ));
                } else if(actual_dec_ty == Ty_record) {
                    if(init_exp.ty != Ty_record || init_exp.ty != Ty_nil)
                        EM_error(d->pos, "expected record type or nil type");
                    if(init_exp.ty == Ty_record && init_exp.ty != actual_dec_ty)
                        EM_error(d->pos, "illegal record variable: expected type %s, but given another initialize type, maybe different reference", S_name(d->u.var.typ));
                } else if(actual_dec_ty->kind != init_exp.ty->kind) {
                    EM_error(d->pos, "expected type %s, but given another initialize type", S_name(d->u.var.typ));
                }
            } else {
                if(init_exp.ty == Ty_nil)
                    EM_error(d->pos, "illegal nil type: nil must be assign to a record type");
            }
            S_enter(venv, d->u.var.var, E_VarEntry(init_exp.ty));
        }
        case A_typeDec: {
            A_nametyList type_list;
            bool cycle_decl = TRUE;
            // example: type list = { first: int, rest: list }
            // 1: add header (type list =) into environment e1
            for(type_list = d->u.type; type_list; type_list = type_list->tail)
                S_enter(tenv, type_list->head->name, Ty_Name(type_list->head->name, NULL));
            // 2: translate body ({ first: int, rest: list }) to fill the place-holder in the e1
            for(type_list = d->u.type; type_list; type_list = type_list->tail) {
                Ty_ty t = transTy(tenv, type_list->head->ty);
                Ty_ty name_type = S_look(tenv, type_list->head->name);
                name_type->u.name.ty = t;
                if(t->kind != Ty_name) cycle_decl = FALSE;
            }
            if(cycle_decl)
                EM_error(d->pos, "illegal cycle type declaration: must contain at least one built-in type");
        }
        case A_functionDec: {
            A_fundecList fun_list;
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
                    if(!r)
                        EM_error(fun_list->head->pos, "undefined return type %s", S_name(fun_list->head->result));
                } else
                    r = Ty_Void();
                
                // parameters
                for(fl = fun_list->head->params; fl; fl = fl->tail) {
                    ty = S_look(tenv, fl->head->typ);
                    if(!ty)
                        EM_error(fl->head->pos, "undefined parameter type %s", S_name(fl->head->typ));
                    if(head) {
                        tail->tail = Ty_TyList(ty, NULL);
                        tail = tail->tail;
                    } else {
                        head = Ty_TyList(ty, NULL);
                        tail = head;
                    }
                }

                S_enter(venv, fun_list->head->name, E_FunEntry(head, r));
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
                Ty_ty result_ty = actual_ty(fun_entry->u.fun.results);
                if(result_ty == Ty_array) {
                    if(exp.ty != Ty_array || exp.ty != result_ty)
                        EM_error(d->pos, "illegal array variable: expected type %s, but given another return type, maybe different reference", S_name(fun_list->head->result));
                } else if(result_ty == Ty_record) {
                    if(exp.ty != Ty_record || exp.ty != Ty_nil)
                        EM_error(d->pos, "expected record type or nil type");
                    if(exp.ty == Ty_record && exp.ty != result_ty)
                        EM_error(d->pos, "illegal record variable: expected type %s, but given another return type, maybe different reference", S_name(fun_list->head->result));
                } else if(result_ty->kind != exp.ty->kind) {
                    EM_error(d->pos, "expected type %s, but given another return type", S_name(fun_list->head->result));
                }
                S_endScope(venv);
            }
        }
    }
}

Ty_ty transTy (S_table tenv, A_ty t) {
    switch(t->kind) {
        case A_nameTy: {
            Ty_ty ty = S_look(tenv, t->u.name);
            if(!ty)
                EM_error(t->pos, "undefined type %s", S_name(t->u.name));
            return ty;
        }
        case A_recordTy: {
            A_fieldList fl;
            Ty_field ty_f;
            Ty_fieldList ty_fl_head = NULL, ty_fl_tail = NULL;
            Ty_ty ty;
            for(fl = t->u.record; fl; fl = fl->tail) {
                ty = S_look(tenv, fl->head->typ);
                if(!ty)
                    EM_error(fl->head->pos, "undefined type %s", S_name(fl->head->typ));
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
            Ty_ty ty = S_look(tenv, t->u.name);
            if(!ty)
                EM_error(t->pos, "undefined type %s", S_name(t->u.name));
            return Ty_Array(ty);
        }
    }
}
