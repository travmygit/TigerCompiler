#ifndef TRANSLATE_H_
#define TRANSLATE_H_

typedef struct Tr_level_ *Tr_level;
typedef struct Tr_access_ *Tr_access;
typedef struct Tr_accessList_ *Tr_accessList;
struct Tr_accessList_ {
    Tr_access head;
    Tr_accessList tail;
};

#endif
