#include "translate.h"
#include <stdio.h>
#include <stdlib.h>
#include "printtree.h"
#include "codegen.h"

struct Tr_level_ {
    Tr_level parent;
	Temp_label name;
	F_frame frame;
	Tr_accessList formals;
};

struct Tr_access_ {
    Tr_level level;
    F_access access;
};

typedef struct patchList_ *patchList;
struct patchList_ {
    Temp_label *head;
    patchList tail;
};
static patchList PatchList(Temp_label *head, patchList tail);
static void doPatch(patchList tList, Temp_label label);
static patchList joinPatch(patchList first, patchList second);

struct Cx {
    patchList trues;
    patchList falses;
    T_stm stm;
};

struct Tr_exp_ {
    enum {
        Tr_ex, Tr_nx, Tr_cx
    } kind;
    union {
        T_exp ex;
        T_stm nx;
        struct Cx cx;
    } u;
};

static Tr_exp Tr_Ex(T_exp ex);
static Tr_exp Tr_Nx(T_stm nx);
static Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm);
static T_exp unEx(Tr_exp e);
static T_stm unNx(Tr_exp e);
static struct Cx unCx(Tr_exp e);

static F_fragList frag_list = NULL;

Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail) {
    Tr_accessList accessList = checked_malloc(sizeof(*accessList));
    accessList->head = head;
    accessList->tail = tail;
    return accessList;
}

Tr_expList Tr_ExpList(Tr_exp head, Tr_expList tail) {
    Tr_expList el = checked_malloc(sizeof(*el));
    el->head = head;
    el->tail = tail;
    return el;
}

static Tr_level outer = NULL; // 库函数和全局变量的level
/**
 * 每次调用都是返回同样的level 就是库函数和全局变量的level
 * 
 */
Tr_level Tr_outermost() {
    if(!outer) {
        outer = Tr_newLevel(NULL, Temp_newlabel(), NULL);
    }
    return outer;
}

static Tr_access Tr_Access(Tr_level level, F_access access) {
    Tr_access local = checked_malloc(sizeof(*local));
    local->level = level;
    local->access = access;
    return local;
}

static Tr_accessList make_formals(Tr_level level) {
	Tr_accessList head = NULL, tail = NULL;
	F_accessList al = F_formals(level->frame)->tail; // ignore head node, 不需要把static link也加进去
                                                     // translate模块只需要在new frame的时候告诉frame添加一个static link
                                                     // 自己不需要处理static link
	for (; al; al = al->tail) {
		Tr_access access = Tr_Access(level, al->head);
		if (head) {
			tail->tail = Tr_AccessList(access, NULL);
			tail = tail->tail;
		} else {
			head = Tr_AccessList(access, NULL);
			tail = head;
		}
	}
	return head;
}

Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals) {
    Tr_level level = checked_malloc(sizeof(*level));
    level->parent = parent;
    level->name = name;
    level->frame = F_newFrame(name, U_BoolList(TRUE, formals)); // 在原formals的基础上多添加一个用作static link
    level->formals = make_formals(level);
    return level;
}

Tr_accessList Tr_formals(Tr_level level) {
    return level->formals;
}

Tr_access Tr_allocLocal(Tr_level level, bool escape) {
    Tr_access local = checked_malloc(sizeof(*local));
    local->level = level;
    local->access = F_allocLocal(level->frame, escape);
    return local;
}

static patchList PatchList(Temp_label *head, patchList tail) {
    patchList pl = checked_malloc(sizeof(*pl));
    pl->head = head;
    pl->tail = tail;
    return pl;
}

static void doPatch(patchList tList, Temp_label label) {
    for(; tList; tList = tList->tail) *(tList->head) = label;
}

static patchList joinPatch(patchList first, patchList second) {
    if(!first) return second;
    for(; first->tail; first = first->tail);
    first->tail = second;
    return first;
}

static Tr_exp Tr_Ex(T_exp ex) {
    Tr_exp e = checked_malloc(sizeof(*e));
    e->kind = Tr_ex;
    e->u.ex = ex;
    return e;
}

static Tr_exp Tr_Nx(T_stm nx) {
    Tr_exp e = checked_malloc(sizeof(*e));
    e->kind = Tr_nx;
    e->u.nx = nx;
    return e;
}

static Tr_exp Tr_Cx(patchList trues, patchList falses, T_stm stm) {
    Tr_exp e = checked_malloc(sizeof(*e));
    e->kind = Tr_cx;
    e->u.cx.stm = stm;
    e->u.cx.trues = trues;
    e->u.cx.falses = falses;
    return e;
}

static T_exp unEx(Tr_exp e) {
    switch(e->kind) {
        case Tr_ex: return e->u.ex;
        case Tr_cx: {
            Temp_temp r = Temp_newtemp();
            Temp_label t = Temp_newlabel(), f = Temp_newlabel();
            doPatch(e->u.cx.trues, t);
            doPatch(e->u.cx.falses, f);
            return T_Eseq(T_Move(T_Temp(r), T_Const(1)),
                    T_Eseq(e->u.cx.stm,
                     T_Eseq(T_Label(f),
                      T_Eseq(T_Move(T_Temp(r), T_Const(0)),
                       T_Eseq(T_Label(t),
                               T_Temp(r))))));
        }
        case Tr_nx: return T_Eseq(e->u.nx, T_Const(0));
    }
    assert(0);
}

static T_stm unNx(Tr_exp e) {
    switch(e->kind) {
        case Tr_ex: return T_Exp(e->u.ex);
        case Tr_cx: {
            Temp_temp r = Temp_newtemp();
            Temp_label t = Temp_newlabel(), f = Temp_newlabel();
            doPatch(e->u.cx.trues, t);
            doPatch(e->u.cx.falses, f);
            return T_Exp(T_Eseq(T_Move(T_Temp(r), T_Const(1)),
                    T_Eseq(e->u.cx.stm,
                     T_Eseq(T_Label(f),
                      T_Eseq(T_Move(T_Temp(r), T_Const(0)),
                       T_Eseq(T_Label(t),
                               T_Temp(r)))))));
        }
        case Tr_nx: return e->u.nx;
    }
    assert(0);
}

static struct Cx unCx(Tr_exp e) {
    switch(e->kind) {
        case Tr_ex: {
            struct Cx cx;
            cx.stm = T_Cjump(T_eq, e->u.ex, T_Const(0), NULL, NULL);
            cx.trues = PatchList(&(cx.stm->u.CJUMP.true), NULL);
            cx.falses = PatchList(&(cx.stm->u.CJUMP.false), NULL);
            return cx;
        }
        case Tr_cx: return e->u.cx;
        case Tr_nx: assert(0);
    }
    assert(0);
}

Tr_exp Tr_noExp() {
    return Tr_Ex(T_Const(0));
}

/**
 * translate frame access to simple varaible
 * 
 * note:
 * access may be declared in outer level, so it needs to follow the static link to find.
 */
Tr_exp Tr_simpleVar(Tr_access access, Tr_level level) {
    T_exp fp = T_Temp(F_FP());
    Tr_level current_level = level;
    // F_formals returns all the formals in the frame, in which the head is static link
    if(access->level != current_level) {
        fp = F_Exp(F_formals(level->frame)->head, fp);
        current_level = current_level->parent;
    }
    return Tr_Ex(F_Exp(access->access, fp));
}

Tr_exp Tr_fieldVar(Tr_exp field, int offset) {
    return Tr_Ex(T_Mem(T_Binop(T_plus, unEx(field), T_Const(offset * F_WORD_SIZE))));
}

Tr_exp Tr_subscriptVar(Tr_exp array, Tr_exp index) {
    return Tr_Ex(T_Mem(T_Binop(T_plus, unEx(array),
                        T_Binop(T_mul, unEx(index), T_Const(F_WORD_SIZE)))));
}

/**
 * nil is treated as a record with no memory space
 * 
 * note:
 * built-in function "initRecord" need a paramater, so called size, to allocate memory space
 */
Tr_exp Tr_nilExp() {
    static Temp_temp nil = NULL;
    if(!nil) {
        nil = Temp_newtemp();
        // initialize nil as a 0
        return Tr_Ex(T_Eseq(T_Move(T_Temp(nil), F_externalCall(String("initRecord"), T_ExpList(T_Const(0), NULL))), T_Temp(nil)));
    }
    return Tr_Ex(T_Temp(nil));
}

Tr_exp Tr_intExp(int n) {
    return Tr_Ex(T_Const(n));
}

/**
 * literal string, 在其他语言中被存储在堆里作为全局字符串
 * 这里维护一个fraglist表示全局字符串, 为每个字符串生成label, 即memory address
 */
static F_fragList global_string = NULL;
Tr_exp Tr_stringExp(string s) {
    Temp_label str = Temp_newlabel();
    F_frag f_string = F_StringFrag(str, s);
    global_string = F_FragList(f_string, global_string);
    return Tr_Ex(T_Name(str));
}

Tr_exp Tr_callExp(Tr_level call_level, Tr_level func_level, Temp_label name, Tr_expList args) {
    T_exp fp = T_Temp(F_FP());
    // follow the static link
    while(call_level != func_level->parent) {
        fp = F_Exp(F_formals(call_level->frame)->head, fp);
        call_level = call_level->parent;
    }
    // convert Tr_expList to T_expList
    Tr_expList el = NULL;
    T_expList head = NULL;
    for(el = args; el; el = el->tail) {
        head = T_ExpList(unEx(el->head), head);
    }
    head = T_ExpList(fp, head);
    return Tr_Ex(T_Call(T_Name(name), head));
}

Tr_exp Tr_arithExp(A_oper op, Tr_exp left, Tr_exp right) {
    T_binOp bop;
    switch(op) {
        case A_plusOp: bop = T_plus; break;
        case A_minusOp: bop = T_minus; break;
        case A_timesOp: bop = T_mul; break;
        case A_divideOp: bop = T_div; break;
        default: printf("%s:%d arithExp cannot handle other operators", __FILE__, __LINE__);
    }
    return Tr_Ex(T_Binop(bop, unEx(left), unEx(right)));
}

Tr_exp Tr_relExp(A_oper op, Tr_exp left, Tr_exp right) {
    T_relOp rop;
    switch(op) {
        case A_eqOp: rop = T_eq; break;
        case A_neqOp: rop = T_ne; break;
        case A_ltOp: rop = T_lt; break;
        case A_leOp: rop = T_le; break;
        case A_gtOp: rop = T_gt; break;
        case A_geOp: rop = T_ge; break;
        default: printf("%s:%d relExp cannot handle other operators", __FILE__, __LINE__);
    }
    T_stm c = T_Cjump(rop, unEx(left), unEx(right), NULL, NULL);
    patchList trues = PatchList(&c->u.CJUMP.true, NULL);
    patchList falses = PatchList(&c->u.CJUMP.false, NULL);
    return Tr_Cx(trues, falses, c);
}

Tr_exp Tr_recordExp(Tr_expList fields, int n_fields) {
    Temp_temp t = Temp_newtemp();
    T_stm record_malloc = T_Move(T_Temp(t), F_externalCall(String("initRecord"), T_ExpList(T_Const(n_fields * F_WORD_SIZE), NULL)));
    // reverse the list
    Tr_expList el = fields, head = NULL;
    for(; el; el = el->tail) {
        head = Tr_ExpList(el->head, head);
    }
    T_stm seq = T_Move(T_Mem(T_Binop(T_plus, T_Temp(t), T_Const(0))), unEx(head->head));
    for(int i = 1; i < n_fields; i++) {
        head = head->tail;
        seq = T_Seq(T_Move(T_Mem(T_Binop(T_plus, T_Temp(t), T_Const(i * F_WORD_SIZE))), unEx(head->head)), seq);
    }
    return Tr_Ex(T_Eseq(T_Seq(record_malloc, seq), T_Temp(t)));
}

Tr_exp Tr_seqExp(Tr_expList seqs) {
    Tr_expList el = seqs;
    T_exp s = unEx(el->head);
    for(el = el->tail; el; el = el->tail) s = T_Eseq(unNx(el->head), s);
    return Tr_Ex(s);
}

Tr_exp Tr_assignExp(Tr_exp left, Tr_exp right) {
    return Tr_Nx(T_Move(unEx(left), unEx(right)));
}

Tr_exp Tr_ifExp(Tr_exp test, Tr_exp then, Tr_exp elsee) {
    Tr_exp con_exp = NULL;

    struct Cx c = unCx(test);
    Temp_label t = Temp_newlabel(), f = Temp_newlabel();
    doPatch(c.falses, f);
    doPatch(c.trues, t);

    if(elsee) {
        /*
        Here, we uses the most simple way to translate if expression,
        The BOOK indicates this way is not efficient when then-else part is Cx
        However, this kind of expression is not so frequently using. So, keep it simple.
        */
        Temp_temp  r = Temp_newtemp();
        Temp_label z = Temp_newlabel();
        T_stm jump = T_Jump(T_Name(z), Temp_LabelList(z, NULL));

        T_stm then_stm = NULL;
        T_stm else_stm = NULL;

        // the expression then and else must have the same type, which is the type of entire if-exp or both produce no value.
        if(elsee->kind == Tr_ex) {
            con_exp = Tr_Ex(T_Eseq(c.stm,
                             T_Eseq(T_Label(t), T_Eseq(T_Move(T_Temp(r), unEx(then)),
                              T_Eseq(jump,
                               T_Eseq(T_Label(f), T_Eseq(T_Move(T_Temp(r), unEx(elsee)),
                                T_Eseq(jump,
                                 T_Eseq(T_Label(z), T_Temp(r))))))))));
        } else {
            then_stm = (then->kind == Tr_nx) ? then->u.nx : then->u.cx.stm;
            else_stm = (elsee->kind == Tr_nx) ? elsee->u.nx : elsee->u.cx.stm;
            con_exp = Tr_Nx(T_Seq(c.stm,
                             T_Seq(T_Label(t), T_Seq(then_stm,
                              T_Seq(jump,
                               T_Seq(T_Label(f), T_Seq(else_stm, 
                                T_Seq(jump,
                                 T_Label(z)))))))));
        }
    } else {
        T_stm then_stm = NULL;

        switch(then->kind) {
            case Tr_ex: then_stm = T_Exp(then->u.ex); break;
            case Tr_nx: then_stm = then->u.nx; break;
            case Tr_cx: then_stm = then->u.cx.stm; break;
        }
        con_exp = Tr_Nx(T_Seq(c.stm, 
                         T_Seq(T_Label(t), T_Seq(then_stm,
                          T_Label(f)))));
    }
    return con_exp;
}

Tr_exp Tr_whileExp(Tr_exp test, Tr_exp done, Tr_exp body) {
    Temp_label test_label = Temp_newlabel(), body_label = Temp_newlabel();
    return Tr_Nx(T_Seq(T_Jump(T_Name(test_label), Temp_LabelList(test_label, NULL)),
                    T_Seq(T_Label(body_label), T_Seq(unNx(body),
                     T_Seq(T_Label(test_label), T_Seq(T_Cjump(T_eq, unEx(test), T_Const(0), unEx(done)->u.NAME, body_label), 
                      T_Label(unEx(done)->u.NAME)))))));
}

Tr_exp Tr_doneExp() {
    Temp_label done_label = Temp_newlabel();
    return Tr_Ex(T_Name(done_label)); // T_Name() return a memory address
}

Tr_exp Tr_breakExp(Tr_exp brk) {
    return Tr_Nx(T_Jump(unEx(brk), Temp_LabelList(unEx(brk)->u.NAME, NULL)));
}

Tr_exp Tr_letExp(Tr_expList exps) {
    Tr_expList l = exps;
    T_stm t = unNx(l->head);
    for(l = l->tail; l; l = l->tail) {
        t = T_Seq(unNx(l->head), t);
    }
    return Tr_Nx(t);
}

Tr_exp Tr_arrayExp(Tr_exp size, Tr_exp init) {
    return Tr_Ex(F_externalCall(String("initArray"), T_ExpList(unEx(size), T_ExpList(unEx(init), NULL))));
}

void Tr_procEntryExit(Tr_level level, Tr_exp body, Tr_accessList formals) {
	F_frag f = F_ProcFrag(unNx(body), level->frame);
	frag_list = F_FragList(f, frag_list);
}

F_fragList Tr_getResult() {
    F_fragList last_string_list = NULL, p = NULL;
    for(p = global_string; p; p = p->tail) {
        last_string_list = p;
    }
    if(last_string_list) {
        last_string_list->tail = frag_list;
    }
    return global_string ? global_string : frag_list;
}

