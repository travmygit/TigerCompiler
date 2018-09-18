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

static Tr_level outer = NULL;

Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail) {
    Tr_accessList accessList = checked_malloc(sizeof(*accessList));
    accessList->head = head;
    accessList->tail = tail;
    return accessList;
}

Tr_level Tr_outermost() {
    if(!outer) outer = Tr_newLevel(NULL, Temp_newlabel(), NULL);
    return outer;
}

Tr_access Tr_Access(Tr_level level, F_access access) {
    Tr_access local = checked_malloc(sizeof(*local));
    local->level = level;
    local->access = access;
    return local;
}

static Tr_accessList make_formals(Tr_level level) {
	Tr_accessList head = NULL, tail = NULL;
	F_accessList al = F_formals(level->frame)->tail; // access parent frame's formals
    
	for (; al; al = al->tail) {
		Tr_access access = Tr_Access(level, al->head);
		if (head) {
			tail->tail = Tr_AccessList(access, NULL);
			tail = tail->tail;
		} else {
			head = Tr_AccessList(access, NULL);
			tail = head;
		}
	}
	return head;
}

Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals) {
    Tr_level level = checked_malloc(sizeof(*level));
    level->parent = parent;
    level->name = name;
    level->frame = F_newFrame(name, U_BoolList(TRUE, formals)); // one room for new bp
    level->formals = make_formals(level);
    return level;
}

Tr_accessList Tr_formals(Tr_level level) {
    return level->formals;
}

Tr_access Tr_allocLocal(Tr_level level, bool escape) {
    Tr_access local = checked_malloc(sizeof(*local));
    local->level = level;
    local->access = F_allocLocal(level->frame, escape);
    return local;
}