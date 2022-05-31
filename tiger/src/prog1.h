#pragma once

#include "slp.h"
#include "util.h"

A_stm prog(void);

// chap1: tells the maximum number of arguments of any print statement within any subexpression of a given statement.
int maxargs(A_stm stm);

// chap1: ¡°interprets¡± a program in this language.
void interp(A_stm stm);
