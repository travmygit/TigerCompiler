#ifndef ERRORMSG_H__
#define ERRORMSG_H__

#include "util.h"

extern bool EM_anyErrors;

void EM_newline(void); // 记录行号以及对应的位置

extern int EM_tokPos;

void EM_error(int, string,...);
void EM_impossible(string,...);
void EM_reset(string filename);

#endif