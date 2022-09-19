#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "parse.h"

extern int yyparse(void);
extern A_exp absyn_root;

A_exp parse(string filename)
{
	EM_reset(filename);
	if (yyparse() == 0)
	{
		fprintf(stderr, "Parsing successful!\n");
		return absyn_root;
	}
	else
	{
		fprintf(stderr, "Parsing failed\n");
		return NULL;
	}
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "usage: a.out filename\n");
		exit(1);
	}
	parse(argv[1]);
	return 0;
}