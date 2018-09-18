#ifndef ENV_H__
#define ENV_H__

#include <stdio.h>
#include "util.h"
#include "types.h"
#include "symbol.h"
#include "temp.h"
#include "translate.h"

typedef struct E_enventry_ *E_enventry;

struct E_enventry_ {
    enum { E_varEntry, E_funEntry } kind;
    union {
        struct { Tr_access access; Ty_ty ty; } var;
        struct { Tr_level level; Temp_label label; Ty_tyList formals; Ty_ty results; } fun;
    } u;
};

/* constructor for value environment */
E_enventry E_VarEntry(Tr_access access, Ty_ty ty);
E_enventry E_FunEntry(Tr_level level, Temp_label label, Ty_tyList formals, Ty_ty results);

S_table E_base_tenv(); /* type environment */
S_table E_base_venv(); /* value environment */

#endif