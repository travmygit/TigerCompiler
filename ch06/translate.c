#include "translate.h"
#include "frame.h"

struct Tr_level_ {
    Tr_level parent;
	Temp_label name;
	F_frame frame;
	Tr_accessList formals;
};

struct Tr_access_ {
    Tr_level level;
    F_access access;
};

