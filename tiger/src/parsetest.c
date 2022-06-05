#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "errormsg.h"

extern int yyparse(void);

void parse(string filename)
{
	EM_reset(filename);
	if (yyparse() == 0)
	{
		fprintf(stderr, "Parsing successful!\n");
	}
	else
	{
		fprintf(stderr, "Parsing failed\n");
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