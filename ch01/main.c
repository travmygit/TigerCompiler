/* This file is intentionally empty.  You should fill it in with your
   solution to the programming exercise. */

#include <stdio.h>
#include "slp.h"
#include "util.h"
#include "interp.h"
#include "maxargs.h"

#include "prog1.h"

int main() {
    /*
     * a := 5+3;
     * b := (print(a, a-1), 10*a);
     * print(b);
     */
    printf("test one: \n");
    printf("maxargs: %d\n", maxargs(prog()));
    interp(prog());
    
    /*
     * a := 5;
     * print(a);
     */
    printf("\ntest two: \n");
    printf("maxargs: %d\n", maxargs(prog_test1()));
    interp(prog_test1());
    
    /*
     * a := 5;
     * print(a, a);
     */
    printf("\ntest three: \n");
    printf("maxargs: %d\n", maxargs(prog_test2()));
    interp(prog_test2());
    
    /*
     * a = 5+3;
     * b = (print(a, (print(a, a, 100, 200, 300), 100), a-1), 10*a);
     * print(b);
     */
    printf("\ntest four: \n");
    printf("maxargs: %d\n", maxargs(prog_test3()));
    interp(prog_test3());
    
    return 0;
}
