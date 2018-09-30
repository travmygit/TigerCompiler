#include "frame.h"


const int F_K = 6;         // not use here yet
const int F_WORD_SIZE = 4;
struct F_frame_ {
    Temp_label name;
    F_accessList formals;
    unsigned int locals;
};

struct F_access_ {
    enum {
        inFrame, inReg
    } kind;
    union {
        int offset;    // in frame
        Temp_temp reg; // in register
    } u;
};
static F_access InFrame(int offset);  // memory location at offset X from the fp
static F_access InReg(Temp_temp reg); // held in reg X
static F_accessList F_AccessList(F_access head, F_accessList tail);

F_frame F_newFrame(Temp_label name, U_boolList formals);
Temp_label F_name(F_frame f);
F_accessList F_formals(F_frame f);
F_access F_allocLocal(F_frame f, bool escape);


F_frame F_newFrame(Temp_label name, U_boolList formals) {
    F_frame f = checked_malloc(sizeof(*f));

    f->name = name;
    f->formals = NULL;
    f->locals = 0;

    U_boolList b;
    int offset = -F_WORD_SIZE; // zero reserved for return address
    // assume all the formals are in frame
    for(b = formals; b; b = b->tail) {
        f->formals = F_AccessList(InFrame(offset), f->formals);
        offset -= F_WORD_SIZE;
        f->locals++;
    }
    return f;
}

Temp_label F_name(F_frame f) {
    return f->name;
}

F_accessList F_formals(F_frame f) {
    return f->formals;
}

F_access F_allocLocal(F_frame f, bool escape) {
    assert(f && escape);
    f->locals++;
    return InFrame(-f->locals * F_WORD_SIZE);
}

static F_accessList F_AccessList(F_access head, F_accessList tail) {
    F_accessList al = checked_malloc(sizeof(*al));
    al->head = head;
    al->tail = tail;
    return al;
}

// indicates a memory location at offset x from the frame pointer
static F_access InFrame(int offset) {
    F_access a = checked_malloc(sizeof(*a));
    a->kind = inFrame;
    a->u.offset = offset;
    return a;
}

// indicates it will be held in register t
static F_access InReg(Temp_temp reg) {
    F_access a = checked_malloc(sizeof(*a));
    a->kind = inReg;
    a->u.reg = reg;
    return a;
}

static Temp_temp fp = NULL;
// IR-tree interface
Temp_temp F_FP() {
    if(!fp) {
        fp = Temp_newtemp();
    }
    return fp;
}

T_exp F_Exp(F_access acc, T_exp framePtr) {
    if(acc->kind == inFrame) {
        return T_Mem(T_Binop(T_plus, framePtr, T_Const(acc->u.offset)));
    } else {
        return T_Temp(acc->u.reg);
    }
}

T_stm F_procEntryExit1(F_frame frame, T_stm stm) {
    return stm;
}

T_exp F_externalCall(string s, T_expList args) {
    return T_Call(T_Name(Temp_namedlabel(s)), args);
}


// fragment implement
F_frag F_StringFrag(Temp_label label, string str) {
    F_frag f = checked_malloc(sizeof(*f));
    f->kind = F_stringFrag;
    f->u.stringg.label = label;
    f->u.stringg.str = str;
    return f;
}

F_frag F_ProcFrag(T_stm body, F_frame frame) {
    F_frag f = checked_malloc(sizeof(*f));
    f->kind = F_procFrag;
    f->u.proc.body = body;
    f->u.proc.frame = frame;
    return f;
}

F_fragList F_FragList(F_frag head, F_fragList tail) {
    F_fragList fl = checked_malloc(sizeof(*fl));
    fl->head = head;
    fl->tail = tail;
    return fl;
}