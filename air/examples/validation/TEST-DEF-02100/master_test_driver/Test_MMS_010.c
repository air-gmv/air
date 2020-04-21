
/** 
 * @file Test_MMS_0010.c
 * 
 *  COPYRIGHT (c) 2013
 *  GMV-SKYSOFT 
 *
 * @author Bruno Oliveira
 * @brief  Test Specific function implementations.
 * 
 */



#include "Test_MMS_010.h"

char TestName[] = "T-API-MMS-010";
char TestDefName[] = "2100";

/*Global variables Instantiation */
SCHEDULE_ID_TYPE SchedId;
SCHEDULE_STATUS_TYPE SchedStatus;
SCHEDULE_NAME_TYPE SchedName;

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: MasterProcedure_T_API_MMS_010                       | *
 * |                                                                    | *
 * | DESCRIPTION:   Test main function implementation, callable from AVT| *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   16/09/2013                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                         Input  Output  Description     | *
 * | ENUM TEST_RESULT_TYPE *TestResult           X     Test result      | *
 * |                                                   SUCCESS or FAILED| *
 * | RETURN VALUE: void                                                 | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
MasterProcedure_T_API_MMS_010 ( TEST_RESULT_TYPE *TestResult )
{  
  RETURN_CODE_TYPE           RetCode  = NO_ERROR;
  BOOLEAN                    FlagOK   = FALSE;

  /*Name index iterator*/
  APEX_BYTE Iter;

  printf("MasterProcedure_T_API_MMS_010\n");

  /* Assign the max, min and acceptable value of SCHEDULE_ID_TYPE SchedId*/
  SchedId = LONG_MAX;
  SchedId = LONG_MIN;
  SchedId = LONG_MAX >> 1;

  /* Iterate trhouth the whole string */
  for(Iter = 0; Iter < MAX_NAME_LENGTH; Iter++) {
    SchedName[Iter] = 'a' + Iter;
  }


  SchedStatus.TIME_OF_LAST_SCHEDULE_SWITCH = (SYSTEM_TIME_TYPE) 1000000ll;
  SchedStatus.CURRENT_SCHEDULE = (SCHEDULE_ID_TYPE) 0;
  SchedStatus.NEXT_SCHEDULE = (SCHEDULE_ID_TYPE) 1;
  
  /* If the application got to this pont, everything is OK */
  FlagOK = TRUE;
  
  /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );

} /* MasterProcedure_T_API_MMS_010 */




/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: Initial                                             | *
 * |                                                                    | *
 * | DESCRIPTION: Function called by PMK for initialization             | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   16/09/2013                                                 | *
 * |                                                                    | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */

void
Initial () {

  TEST_RESULT_TYPE TestResult;
  RETURN_CODE_TYPE RetCode = NO_ERROR;

  w_SET_PARTITION_MODE ( NORMAL, &RetCode );

  MasterProcedure_T_API_MMS_010( &TestResult );


  STOP_SELF();

}


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Test_PART_230_0060.c                    | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
