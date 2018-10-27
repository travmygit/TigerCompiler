/* frame.h */
#ifndef FRAME_H
#define FRAME_H

#include "util.h"
#include "temp.h"
#include "tree.h"
#include "assem.h"

extern const int F_WORD_SIZE;

typedef struct F_frame_ *F_frame;
typedef struct F_access_ *F_access;

typedef struct F_accessList_ *F_accessList;
struct F_accessList_ {
    F_access head;
    F_accessList tail;
};

/**
 * malloc a new frame
 */
F_frame F_newFrame(Temp_label name, U_boolList formals);
/**
 * return frame's address
 */
Temp_label F_name(F_frame f);
/**
 * fetch frame's formal list
 */
F_accessList F_formals(F_frame f);
/**
 * alloc a new local varibles in the frame
 */
F_access F_allocLocal(F_frame f, bool escape);

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

/**
 * utils for translate module
 */
T_exp F_Exp(F_access acc, T_exp framePtr);
T_exp F_externalCall(string s, T_expList args);

/**
 * MIPS calling convention
 * 
 * the $v registers are for function returns,
 * the $a registers are for function arguments,
 * the $t variables are temporary caller saved registers,
 * the $s registers are callee saved.
*/

Temp_map F_tempMap();

/**
 * normal register in mips
 */
Temp_temp F_FP();
Temp_temp F_RV();
Temp_temp F_RA();
Temp_temp F_SP();
Temp_temp F_ZERO();
Temp_tempList F_CalleeSaves();
Temp_tempList F_CallerSaves();
Temp_tempList F_ArgsRegs();
Temp_tempList F_CallSaves();

/**
 * proceduce entry exit
 * 
 * 1:
 * 2:
 * 3:
 */
T_stm        F_procEntryExit1(F_frame frame, T_stm stm);
AS_instrList F_procEntryExit2(AS_instrList body);
AS_proc      F_procEntryExit3(F_frame frame, AS_instrList body);

#endif