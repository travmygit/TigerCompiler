#include "env.h"

E_enventry E_VarEntry(Tr_access access, Ty_ty ty) {
    E_enventry entry = checked_malloc(sizeof(*entry));
    entry->u.var.access = access;
    entry->u.var.ty = ty;
    entry->kind = E_varEntry;
    return entry;
}

E_enventry E_FunEntry(Tr_level level, Temp_label label, Ty_tyList formals, Ty_ty results) {
    E_enventry entry = checked_malloc(sizeof(*entry));
    entry->u.fun.level = level;
    entry->u.fun.label = label;
    entry->u.fun.formals = formals;
    entry->u.fun.results = results;
    entry->kind = E_funEntry;
    return entry;
}

S_table E_base_tenv() {
    S_table tenv = S_empty();
    S_enter(tenv, S_Symbol("int"), Ty_Int());
    S_enter(tenv, S_Symbol("string"), Ty_String());
    S_enter(tenv, S_Symbol("nil"), Ty_Nil());
    return tenv;
}

S_table E_base_venv() {
    S_table venv = S_empty();

    // built-in function environment entry
    E_enventry env_print = E_FunEntry(Tr_outermost(), Temp_newlabel(),
		Ty_TyList(Ty_String(), NULL), Ty_Void());
	E_enventry env_flush = E_FunEntry(Tr_outermost(), Temp_newlabel(),
		NULL, Ty_Void());
	E_enventry env_getchar = E_FunEntry(Tr_outermost(), Temp_newlabel(),
		NULL, Ty_String());
	E_enventry env_ord = E_FunEntry(Tr_outermost(), Temp_newlabel(),
		Ty_TyList(Ty_String(), NULL), Ty_Int());
	E_enventry env_chr = E_FunEntry(Tr_outermost(), Temp_newlabel(),
		Ty_TyList(Ty_Int(), NULL), Ty_String());
	E_enventry env_size = E_FunEntry(Tr_outermost(), Temp_newlabel(),
		Ty_TyList(Ty_String(), NULL), Ty_Int());
	E_enventry env_substring = E_FunEntry(Tr_outermost(), Temp_newlabel(),
		Ty_TyList(Ty_String(), Ty_TyList(Ty_Int(),
					Ty_TyList(Ty_Int(), NULL))), Ty_String());
	E_enventry env_concat = E_FunEntry(Tr_outermost(), Temp_newlabel(),
		Ty_TyList(Ty_String(), Ty_TyList(Ty_String(), NULL)), Ty_String());
	E_enventry env_not = E_FunEntry(Tr_outermost(), Temp_newlabel(),
		Ty_TyList(Ty_Int(), NULL), Ty_Int());
	E_enventry env_exit = E_FunEntry(Tr_outermost(), Temp_newlabel(),
		Ty_TyList(Ty_Int(), NULL), Ty_Void());

    S_enter(venv, S_Symbol("print"), env_print);
    S_enter(venv, S_Symbol("flush"), env_flush);
    S_enter(venv, S_Symbol("getchar"), env_getchar);
    S_enter(venv, S_Symbol("ord"), env_ord);
    S_enter(venv, S_Symbol("chr"), env_chr);
    S_enter(venv, S_Symbol("size"), env_size);
    S_enter(venv, S_Symbol("substring"), env_substring);
    S_enter(venv, S_Symbol("concat"), env_concat);
    S_enter(venv, S_Symbol("not"), env_not);
    S_enter(venv, S_Symbol("exit"), env_exit);
    return venv;
}
