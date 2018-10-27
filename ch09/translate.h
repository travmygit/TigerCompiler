#ifndef TRANSLATE_H_
#define TRANSLATE_H_

#include "absyn.h"
#include "temp.h"
#include "frame.h"
#include "tree.h"
#include "canon.h"

typedef struct Tr_level_ *Tr_level;
typedef struct Tr_access_ *Tr_access;
typedef struct Tr_accessList_ *Tr_accessList;
struct Tr_accessList_ {
    Tr_access head;
    Tr_accessList tail;
};
Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail);

typedef struct Tr_exp_ *Tr_exp;
typedef struct Tr_expList_ *Tr_expList;
struct Tr_expList_ {
    Tr_exp head;
    Tr_expList tail;
};
Tr_expList Tr_ExpList(Tr_exp head, Tr_expList tail);

/**
 * 与frame模块交互的接口
 */
Tr_level Tr_outermost();
Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals);
Tr_access Tr_allocLocal(Tr_level level, bool escape);
Tr_accessList Tr_formals(Tr_level level);

F_fragList Tr_getResult();
void Tr_procEntryExit(Tr_level level, Tr_exp body, Tr_accessList formals);

/**
 * 与semant模块交互的接口
 */
Tr_exp Tr_noExp();
Tr_exp Tr_nilExp();
Tr_exp Tr_simpleVar(Tr_access access, Tr_level level);
Tr_exp Tr_fieldVar(Tr_exp array, int offset);
Tr_exp Tr_subscriptVar(Tr_exp array, Tr_exp index);
Tr_exp Tr_intExp(int n);
Tr_exp Tr_stringExp(string s);
Tr_exp Tr_callExp(Tr_level call_level, Tr_level func_level, Temp_label name, Tr_expList args);
Tr_exp Tr_arithExp(A_oper op, Tr_exp left, Tr_exp right);
Tr_exp Tr_relExp(A_oper op, Tr_exp left, Tr_exp right);
Tr_exp Tr_recordExp(Tr_expList fields, int n_fields);
Tr_exp Tr_seqExp(Tr_expList seqs);
Tr_exp Tr_assignExp(Tr_exp left, Tr_exp right);
Tr_exp Tr_ifExp(Tr_exp test, Tr_exp then, Tr_exp elsee);
Tr_exp Tr_whileExp(Tr_exp test, Tr_exp done, Tr_exp body);
Tr_exp Tr_doneExp();
Tr_exp Tr_breakExp(Tr_exp brk);
Tr_exp Tr_letExp(Tr_expList exps);
Tr_exp Tr_arrayExp(Tr_exp size, Tr_exp init);

#endif
