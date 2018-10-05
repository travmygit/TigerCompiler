#include "frame.h"

const int F_K = 6;         // maximum number of arguments that can be stored in register
const int F_WORD_SIZE = 4; // MIPS's a WORD size is 4 bytes

struct F_frame_ {
    Temp_label name;       // frame's label, use for assembly code
    F_accessList formals;  // frame's local variables
    unsigned int locals;   // frame's local variables' number
};

struct F_access_ {
    enum {
        inFrame, inReg
    } kind;
    union {
        int offset;    // arguments in frame, the offset of variables
        Temp_temp reg; // arguments in register, specify register
    } u;
};

static F_access InFrame(int offset);
static F_access InReg(Temp_temp reg);
static F_accessList F_AccessList(F_access head, F_accessList tail);

F_frame F_newFrame(Temp_label name, U_boolList formals);
Temp_label F_name(F_frame f);
F_accessList F_formals(F_frame f);
F_access F_allocLocal(F_frame f, bool escape);

/**
 * new frame for mips
 * 
 * note:
 * fp+0  return address
 * fp+4  static link
 * fp+8  first arguments
 * fp+C  second arguments
 */
F_frame F_newFrame(Temp_label name, U_boolList formals) {
    F_frame f = checked_malloc(sizeof(*f));
    int offset = -F_WORD_SIZE; // zero reserved for return address

    f->name = name;
    f->formals = F_AccessList(InFrame(offset), NULL); // put in static link in the HEAD
    f->locals = 1;

    // to keep things easy, all variables are escaping...
    U_boolList b = NULL;
    F_accessList new_node = NULL, node = f->formals;
    for(b = formals->tail; b; b = b->tail) {
        offset -= F_WORD_SIZE;
        new_node = F_AccessList(InFrame(offset), NULL);
        node->tail = new_node;
        node = node->tail;
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
    return InFrame(-(f->locals * F_WORD_SIZE));
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

/**
 * a global fp stand for the frame pointer register-it's presented as ebp in x86 architecture-
 * which point to the current level's frame.
 */
static Temp_temp fp = NULL;
/**
 * always return a fp register point to current level's frame.
 */
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
