#include "util.h"
#include "slp.h"

//A_stm prog(void) {
//
//return
// A_CompoundStm(A_AssignStm("a",
//                 A_OpExp(A_NumExp(5), A_plus, A_NumExp(3))),
//  A_CompoundStm(A_AssignStm("b",
//      A_EseqExp(A_PrintStm(A_PairExpList(A_IdExp("a"),
//                 A_LastExpList(A_OpExp(A_IdExp("a"), A_minus,
//                                       A_NumExp(1))))),
//              A_OpExp(A_NumExp(10), A_times, A_IdExp("a")))),
//   A_PrintStm(A_LastExpList(A_IdExp("b")))));
//}

/*
 * a := 5+3;
 * b := (print(a, a-1), 10*a);
 * print(b);
 */
A_stm prog(void) {
    
    return
    A_CompoundStm(
                  A_AssignStm(
                              "a",
                              A_OpExp(
                                      A_NumExp(5),
                                      A_plus,
                                      A_NumExp(3)
                                      )
                              ),
                  A_CompoundStm(
                                A_AssignStm(
                                            "b",
                                            A_EseqExp(
                                                      A_PrintStm(
                                                                 A_PairExpList(
                                                                               A_IdExp(
                                                                                       "a"
                                                                                       ),
                                                                               A_LastExpList(
                                                                                             A_OpExp(
                                                                                                     A_IdExp("a"),
                                                                                                     A_minus,
                                                                                                     A_NumExp(1)
                                                                                                     )
                                                                                             )
                                                                               )
                                                                 ),
                                                      A_OpExp(
                                                              A_NumExp(10),
                                                              A_times,
                                                              A_IdExp("a")
                                                              )
                                                      )
                                            ),
                                A_PrintStm(
                                           A_LastExpList(
                                                         A_IdExp("b")
                                                         )
                                           )
                                )
                  );
    
}


/*
 * a := 5;
 * print(a);
 */
A_stm prog_test1(void) {
    
    return
    A_CompoundStm(
                  A_AssignStm(
                              "a",
                              A_NumExp(5)
                              ),
                  A_PrintStm(
                             A_LastExpList(
                                           A_IdExp("a")
                                           )
                             )
                  );
    
}


/*
 * a := 5;
 * print(a, a);
 */
A_stm prog_test2(void) {
    
    return
    A_CompoundStm(
                  A_AssignStm(
                              "a",
                              A_NumExp(5)
                              ),
                  A_PrintStm(
                             A_PairExpList(
                                           A_IdExp("a"),
                                           A_LastExpList(
                                                         A_IdExp("a")
                                                         )
                                           )
                             )
                  );
    
}


/*
 * a = 5+3;
 * b = (print(a, (print(a, a, 100, 200, 300), 100), a-1), 10*a);
 * print(b);
 */
A_stm
prog_test3(void) {
    
    return
    A_CompoundStm(
                  A_AssignStm(
                              "a",
                              A_OpExp(
                                      A_NumExp(5),
                                      A_plus,
                                      A_NumExp(3)
                                      )
                              ),
                  A_CompoundStm(
                                A_AssignStm(
                                            "b",
                                            A_EseqExp(
                                                      A_PrintStm(
                                                                 A_PairExpList(
                                                                               A_IdExp("a"),
                                                                               A_PairExpList(
                                                                                             A_EseqExp(
                                                                                                       A_PrintStm(
                                                                                                                  A_PairExpList(
                                                                                                                                A_IdExp("a"),
                                                                                                                                A_PairExpList(
                                                                                                                                              A_IdExp("a"),
                                                                                                                                              A_PairExpList(
                                                                                                                                                            A_NumExp(100),
                                                                                                                                                            A_PairExpList(A_NumExp(200),
                                                                                                                                                                          A_LastExpList(A_NumExp(300))
                                                                                                                                                                          )
                                                                                                                                                            )
                                                                                                                                              )
                                                                                                                                )
                                                                                                                  ),
                                                                                                       A_NumExp(100)
                                                                                                       ),
                                                                                             A_LastExpList(
                                                                                                           A_OpExp(
                                                                                                                   A_IdExp("a"),
                                                                                                                   A_minus,
                                                                                                                   A_NumExp(1)
                                                                                                                   )
                                                                                                           )
                                                                                             )
                                                                               )
                                                                 ),
                                                      A_OpExp(
                                                              A_NumExp(10),
                                                              A_times,
                                                              A_IdExp("a")
                                                              )
                                                      )
                                            ),
                                A_PrintStm(
                                           A_LastExpList(
                                                         A_IdExp("b")
                                                         )
                                           )
                                )
                  );
}
