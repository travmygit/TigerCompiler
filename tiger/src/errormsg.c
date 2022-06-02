#include "errormsg.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "util.h"

extern FILE* yyin;

bool EM_anyErrors = FALSE;
int  EM_tokPos = 0;

static string    EM_fileName = "";
static int       EM_lineNum = 1;
static U_intList EM_linePos = NULL;

void EM_newline(void)
{
	EM_lineNum++;
	EM_linePos = U_IntList(EM_tokPos, EM_linePos);
}

void EM_error(int pos, string message, ...)
{
	U_intList lines = EM_linePos;
	int num = EM_lineNum;
	EM_anyErrors = TRUE;
	while (lines && lines->head >= pos)
	{
		lines = lines->tail; num--;
	}
	if (EM_fileName)
	{
		fprintf(stderr, "%s:", EM_fileName);
	}
	if (lines)
	{
		fprintf(stderr, "%d.%d: ", num, pos - lines->head);
	}

	va_list ap;
	va_start(ap, message);
	vfprintf(stderr, message, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}

void EM_impossible(string message, ...)
{
	va_list ap;
	va_start(ap, message);
	vfprintf(stderr, message, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}

void EM_reset(string filename)
{
	EM_anyErrors = FALSE;
	EM_fileName = filename;
	EM_lineNum = 1;
	EM_linePos = U_IntList(0, NULL);

	yyin = fopen(filename, "r");
	if (!yyin)
	{
		EM_error(0, "cannot open");
		exit(1);
	}
}