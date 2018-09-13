/* frame.h */
#ifndef FRAME_H
#define FRAME_H

#include "util.h"
#include "temp.h"

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

#endif