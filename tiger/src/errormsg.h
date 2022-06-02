#include "util.h"

extern bool EM_anyErrors;
extern int  EM_tokPos;

void EM_newline(void);
void EM_error(int, string, ...);
void EM_impossible(string, ...);
void EM_reset(string filename);
