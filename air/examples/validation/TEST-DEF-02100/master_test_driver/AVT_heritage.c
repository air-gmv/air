/** 
 * @file AVT_heritage.c
 * 
 *  COPYRIGHT (c) 2013
 *  GMV-SKYSOFT 
 *
 * @author Bruno Oliveira
 * @brief  AVT functions equivalent implementations and missing ARINC 
 *    functions workaround.
 * @note The functions contained in this file should be compatible with 
 *    other AVT implementations, i.e., this functions should make it
 *    possible to integrate the test code in an AVT implementation
 * 
 */



#include "AVT_heritage.h"


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: print_testdef_result                                     | *
 * |                                                                    | *
 * | DESCRIPTION: Print Test results based on TEST_RESULT_TYPE input    | *
 * |              using GMV TESTDEF format                              | *
 * | AUTHOR: Bruno Oliveira                                             | *
 * | DATE:   30/10/2013                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * |                                                                    | *
 * | INPUT:  TEST_RESULT_TYPE TestResult                                | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */

void print_testdef_result(TEST_RESULT_TYPE TestResult) {
  extern char TestDefName[];
  printf("####%s####\n", TestDefName);
  switch (TestResult) {
    case FAILED:
    case NOT_PERFORMED:
      printf("**0**\n");
    break;
    case SUCESS:
      printf("**1**\n");
    break;
    default:
      printf("Error in print_testdef_result\n");
    break;
  }
}


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: EvaluateResult                                      | *
 * |                                                                    | *
 * | DESCRIPTION: Receive a boolean value, and return OK or FAIL        | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | BOOLEAN FlagOK                  X                                  | *
 * |                                                                    | *
 * | RETURN VALUE:  TEST_RESULT_TYPE TestResult                         | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
TEST_RESULT_TYPE EvaluateResult( BOOLEAN FlagOK )
{
  int TestResult;

  if( FlagOK == TRUE )
  {
    TestResult = SUCESS;
    printf("SUCCESS\n");
    print_testdef_result(SUCESS);
  }
  else
  {
    TestResult = FAILED;
    printf("FAILED\n");
    print_testdef_result(FAILED);
  }
  
  return TestResult;

} /* EvaluateResult */




/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_SET_PARTITION_MODE                                | *
 * |                                                                    | *
 * | DESCRIPTION: Execute  GET_PARTITION_MODE with trace                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | Like  GET_PARTITION_STATUS                                         | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */

void
w_SET_PARTITION_MODE( OPERATING_MODE_TYPE OperMode,
          RETURN_CODE_TYPE    *RetCode )
{
  char *TabOper[] = { "IDLE", "COLD_STAR", "WARM_START", "NORMAL" };

  
  printf("w_SET_PARTITION_MODE (%d)\n", OperMode);
  /* execute service */
  SET_PARTITION_MODE( OperMode, RetCode );

}  /* w_SET_PARTITION_MODE */





/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: STOP_SELF                                           | *
 * |                                                                    | *
 * | DESCRIPTION: Equivalent implementation for AIR specific target     | *
 * |            implementation                                          | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | Like  GET_PARTITION_STATUS                                         | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */

void STOP_SELF( void ) {
  pmk_system_halt();
}
