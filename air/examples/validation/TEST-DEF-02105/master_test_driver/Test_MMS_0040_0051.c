/** 
 * @file Test_MMS_0040_0011.c
 * 
 *  COPYRIGHT (c) 2013
 *  GMV-SKYSOFT 
 *
 * @author Bruno Oliveira
 * @brief  Test Specific function implementations.
 * 
 */



#include "Test_MMS_0040_0051.h"
#include "AVT_MMS.h"


/* force size to be 12 */
/* implementation dependent */
APEX_LONG_INTEGER Correct_Order[STACK_SIZE] = 
	{1, 2, 1, 2, 1, 2, 0, 0, 0, 0, 0, 0};
int Number_of_Cycles = 0;
char TestName[] = "Test_MMS_0040_0051";
char TestDefName[] = "2105";


void SetErrorBits(Matching_Error_Bits *ReturnStruct) {
  ReturnStruct->Cond5 = FALSE;
}


void SetNoErrorBits(
  SCHEDULE_ID_TYPE SCHEDULE_ID,
  Matching_Error_Bits *ReturnStruct,
  int count) {

  if (CheckPartitionStatus(SCHEDULE_ID, count)) {
    ReturnStruct->Cond5 = TRUE;
  } else {
    printf("!Bad Partition Status!\n");
    ReturnStruct->Cond5 = FALSE;
  }
}





/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CheckPartitionStatus                                | *
 * |                                                                    | *
 * | DESCRIPTION: Check if the last 3 partitions parameters             | *
 * |      received via queued messages is correct                       | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR: Bruno Oliveira                                             | *
 * | DATE:   23/09/2013                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | * 
 * |                                                                    | *
 * | RETURN VALUE:  BOOLEAN                                             | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */

BOOLEAN CheckPartitionStatus( SCHEDULE_ID_TYPE SCHEDULE_ID, int count ) {

  return TRUE;

}




void MasterProcedure_T_API_MMS_0040_0051( TEST_RESULT_TYPE *TestResult ) {

	RETURN_CODE_TYPE RetCode = NO_ERROR;
  Matching_Error_Bits ReturnStruct;
	BOOLEAN FlagOK = FALSE;
  extern BOOLEAN Test_Not_Over;
  extern APEX_INTEGER CycleCounter;

  /*Call procedure CheckSetModuleSchedule with the following argument:
    - SCHEDULE_ID = 5

    INVALID_CONFIG as expected return code.
  */
  Number_of_Cycles = 3;

	CheckSetModuleSchedule(5, &ReturnStruct, &RetCode);

	if ( (RetCode == INVALID_PARAM) && 
      ReturnStruct.Cond5 == TRUE ) {
    printf("Return INVALID_CONFIG\n");
		FlagOK = TRUE;
	} else {
    FlagOK = FALSE;
  }

	/* Fill return value */
  *TestResult = EvaluateResult( FlagOK );

}






void
Initial () {

  TEST_RESULT_TYPE TestResult;
  RETURN_CODE_TYPE RetCode = NO_ERROR;
  QUEUING_PORT_ID_TYPE PortId;

  w_CREATE_QUEUING_PORT(
  	"Test_1_Dq", 
  	STR_SIZE,
  	4,
  	DESTINATION,
  	FIFO,
  	&PortId,
  	&RetCode);

  w_CREATE_QUEUING_PORT(
  	"Test_2_Dq", 
  	STR_SIZE,
  	4,
  	DESTINATION,
  	FIFO,
  	&PortId,
  	&RetCode);

  if ( PARTITION_3_PRESENT ) {
    w_CREATE_QUEUING_PORT(
    "Test_3_Dq", 
    STR_SIZE,
    4,
    DESTINATION,
    FIFO,
    &PortId,
    &RetCode);
  }

  w_SET_PARTITION_MODE ( NORMAL, &RetCode );

  MasterProcedure_T_API_MMS_0040_0051( &TestResult );

  print_test_results (TestResult);
  print_testdef_result(TestResult);

  w_STOP_SELF();

}
