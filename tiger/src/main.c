#include <stdio.h>
#include "util.h"
#include "slp.h"
#include "prog1.h"

int main(int argc, char** argv)
{
	printf("Welcome to tiger compiler!\n");

	// chap1
	A_stm stm = prog();
	printf("---chap1---\n");
	printf("maxargs:\n%d\n", maxargs(stm));
	printf("interp:\n");
	interp(stm);

	return 0;
}