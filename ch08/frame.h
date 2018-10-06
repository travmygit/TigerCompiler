/* frame.h */
#ifndef FRAME_H
#define FRAME_H

#include "util.h"
#include "temp.h"
#include "tree.h"

typedef struct F_frame_ *F_frame;
typedef struct F_access_ *F_access;

typedef struct F_accessList_ *F_accessList;
struct F_accessList_ {
    F_access head;
    F_accessList tail;
};

// 1. how the parameter will be seen from inside the function
// 2. what instructions must be produced to implement the 'view shift'
// such as: copy the stack pointer to frame pointer
F_frame F_newFrame(Temp_label name, U_boolList formals);

// get the function name
Temp_label F_name(F_frame f);

// extracts a list of k 'accesses' denoting the locations where the formal parameters will be kept at run time
F_accessList F_formals(F_frame f);


F_access F_allocLocal(F_frame f, bool escape);

// Translate module, fragment
typedef struct F_frag_ *F_frag;
struct F_frag_ {
    enum {
        F_stringFrag, F_procFrag
    } kind;
    union {
        struct { Temp_label label; string str; } stringg;
        struct { T_stm body; F_frame frame; } proc;
    } u;
};
F_frag F_StringFrag(Temp_label label, string str);
F_frag F_ProcFrag(T_stm body, F_frame frame);

typedef struct F_fragList_ *F_fragList;
struct F_fragList_ {
    F_frag head;
    F_fragList tail;
};
F_fragList F_FragList(F_frag head, F_fragList tail);

// IR-tree interface
Temp_temp F_FP();
extern const int F_WORD_SIZE;
T_exp F_Exp(F_access acc, T_exp framePtr);
T_stm F_procEntryExit1(F_frame frame, T_stm stm);

T_exp F_externalCall(string s, T_expList args);

#endif