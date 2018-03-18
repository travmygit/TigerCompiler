#include "maxargs.h"
#include <stdlib.h>


int maxargs(A_stm stm) {
    assert(stm != NULL);
    if(stm->kind == A_assignStm) {
        if(stm->u.assign.exp->kind == A_eseqExp) {
            return maxargs(stm->u.assign.exp->u.eseq.stm);
        } else
        	return 0;
    } else if(stm->kind == A_compoundStm) {
        int t1 = maxargs(stm->u.compound.stm1), t2 = maxargs(stm->u.compound.stm2);
        return t1 > t2 ? t1 : t2;
    } else if(stm->kind == A_printStm) {
        A_expList list = stm->u.print.exps;
        if(list->kind == A_lastExpList){
            int args_num = 1;
            int call_num = 0;
            if(list->u.last->kind == A_eseqExp)
                call_num = maxargs(list->u.last->u.eseq.stm);
            return call_num > args_num ? call_num : args_num;
        } else {
            int args_num = 1;
            int call_num = 0;
            int max_call_num = 0;
            while(list->kind != A_lastExpList) {
                if(list->u.pair.head->kind == A_eseqExp) {
                    call_num = maxargs(list->u.pair.head->u.eseq.stm);
                    if(call_num > max_call_num) max_call_num = call_num;
                }
                list = list->u.pair.tail;
                args_num++;
            }
            return max_call_num > args_num ? max_call_num : args_num;
        }
    } else {
        // syntax wrong!
        assert(!"Syntax Error");
    }
}
