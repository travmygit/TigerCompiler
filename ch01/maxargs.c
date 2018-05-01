#include "maxargs.h"
#include <stdlib.h>

/* Tells the maximum number of arguments of any print statement
   within any subexpression of a given statement. */
int maxargs(A_stm stm) {
  // NULL stm
  assert(stm != NULL);

  if(stm->kind == A_assignStm) {
    // assign stm
    if(stm->u.assign.exp->kind == A_eseqExp) {
      return maxargs(stm->u.assign.exp->u.eseq.stm);
    } else
    	return 0;
  } else if(stm->kind == A_compoundStm) {
    // compound stm
    int t1 = maxargs(stm->u.compound.stm1), t2 = maxargs(stm->u.compound.stm2);
    return t1 > t2 ? t1 : t2;
  } else if(stm->kind == A_printStm) {
    // print stm
    A_expList list = stm->u.print.exps;
    if(list->kind == A_lastExpList){
      int substm_num = 0;
      // calculate substatement print argc
      if(list->u.last->kind == A_eseqExp)
        substm_num = maxargs(list->u.last->u.eseq.stm);
      return substm_num > 1 ? substm_num : 1;
    } else {
      int args_num = 1;        // this stm arguments
      int substm_num = 0;      // child stm arguments
      int max_substm_num = 0;  // max child stm arguments
      while(list->kind != A_lastExpList) {
        if(list->u.pair.head->kind == A_eseqExp) {
            substm_num = maxargs(list->u.pair.head->u.eseq.stm);
            if(substm_num > max_substm_num) max_substm_num = substm_num;
        }
        list = list->u.pair.tail;
        args_num++;
      }
      return max_substm_num > args_num ? max_substm_num : args_num;
    }
  } else {
    // unknown stm
    assert(!"Unknown statement");
  }
}
