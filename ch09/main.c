#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "temp.h"
#include "tree.h"
#include "assem.h"
#include "frame.h"
#include "semant.h"
#include "canon.h"
#include "prabsyn.h"
#include "printtree.h"
#include "escape.h"
#include "parse.h"
#include "codegen.h"

static void do_proc(FILE *out, F_frame frame, T_stm body)
{
    AS_instrList instr_l = NULL;
    T_stmList stm_l = NULL;

    /* dumping map */
    // printf("Mapping\n");
    // Temp_dumpMap(out, F_tempMap());
    // printf("\n---------\n");

    stm_l   = C_linearize(body);
    stm_l   = C_traceSchedule(C_basicBlocks(stm_l));
    /* canon tree */
    // printf("Canon tree\n");
    // printStmList(out, stm_l);
    // printf("\n---------\n");

    /* assembly */
    instr_l = F_codegen(frame, stm_l);
    AS_printInstrList(out, instr_l, F_tempMap());
}

static void do_string(FILE *out, F_frame frame, T_stm body)
{

}

int main(int argc, char *argv[])
{
    if(argc != 2) {
        fprintf(stderr, "usage: a.out filename\n");
        return 1;
    }

    A_exp absyn_root = parse(argv[1]);
    if(absyn_root == NULL) {
        fprintf(stderr, "something wrong with parser\n");
        return 1;
    }

    FILE *out = stdout;
    /* Absyn tree */
    fprintf(out, "========== Absyn Tree ==========\n");
    pr_exp(out, absyn_root, 0);
    fprintf(out, "\n========== End ==========\n\n");

    /* Set escape varibles */
    Esc_findEscape(absyn_root);

    /* Semantic and Translate */
    F_fragList frags = SEM_transProg(absyn_root);

    /* Assembly code */
    fprintf(out, "========== ASM  ==========\n");
    /* proc */
    for(F_fragList f = frags; f; f = f->tail) {
        if(f->head->kind == F_procFrag) {
            do_proc(out, f->head->u.proc.frame, f->head->u.proc.body);
        }
    }
    /* string */
    for(F_fragList f = frags; f; f = f->tail) {
        if(f->head->kind == F_stringFrag) {
            do_string(out, f->head->u.proc.frame, f->head->u.proc.body);
        }
    }
    fprintf(out, "\n========== End ==========\n\n");

    return 0;
}



