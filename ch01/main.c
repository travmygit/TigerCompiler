/* This file is intentionally empty.  You should fill it in with your
   solution to the programming exercise. */

#include <stdio.h>
#include "slp.h"
#include "util.h"
#include "interp.h"
#include "maxargs.h"
#include "prog.h"

int main() {
    /*
     * a := 5+3;
     * b := (print(a, a-1), 10*a);
     * print(b);
     */
    printf("#TEST1: \n");
    printf("maxargs: %d\n", maxargs(prog()));
    printf("interpret: \n");
    interp(prog());

    return 0;
}
