#include "env.h"

E_enventry E_VarEntry(Ty_ty ty) {
    E_enventry entry = checked_malloc(sizeof(*entry));
    entry->u.var.ty = ty;
    return entry;
}

E_enventry E_FunEntry(Ty_tyList formals, Ty_ty results) {
    E_enventry entry = checked_malloc(sizeof(*entry));
    entry->u.fun.formals = formals;
    entry->u.fun.results = results;
    return entry;
}

S_table E_base_tenv() {
    S_table tenv = S_empty();
    S_enter(tenv, S_Symbol("int"), Ty_Int());
    S_enter(tenv, S_Symbol("string"), Ty_String());
    S_enter(tenv, S_Symbol("nil"), Ty_Nil());
}

S_table E_base_venv() {
    S_table venv;
    S_enter(venv, S_Symbol("print"), Ty_Void());
    S_enter(venv, S_Symbol("flush"), Ty_Void());
    S_enter(venv, S_Symbol("getchar"), Ty_String());
    S_enter(venv, S_Symbol("ord"), Ty_Int());
    S_enter(venv, S_Symbol("chr"), Ty_String());
    S_enter(venv, S_Symbol("size"), Ty_Int());
    S_enter(venv, S_Symbol("substring"), Ty_String());
    S_enter(venv, S_Symbol("concat"), Ty_String());
    S_enter(venv, S_Symbol("not"), Ty_Int());
    S_enter(venv, S_Symbol("exit"), Ty_Void());
}
