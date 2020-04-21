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



#include "Test_MMS_0040_0031.h"
#include "AVT_MMS.h"


BOOLEAN Part3InSched[NUM_OF_SCHEDS] = {FALSE, TRUE};

/* force size to be 12 */
/* implementation dependent */
APEX_LONG_INTEGER Correct_Order[STACK_SIZE] = 
	{1, 2, 2, 1, 2, 3, 2, 3, 1, -1, -1, -1};
char TestName[] = "Test_MMS_0040_0031";
char TestDefName[] = "2103";



extern char *Message_Addr;



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

BOOLEAN CheckPartitionStatus( int count ) {

  extern char Message_Stack[STACK_SIZE][STR_SIZE];

  /*
  Queued Messages order:
  0 - Partition ID
  1 - Operating Mode
  2 - Period
  3 - Duration
  4 - Start Condition
  5 - System Time 
  */

  APEX_LONG_INTEGER *fStack = NULL;

  /* Search The first Message from partition 3 */
  do {
    count--;
    fStack = (APEX_LONG_INTEGER *)&(Message_Stack[count][0]);
    } while ( fStack[0] != (APEX_LONG_INTEGER)3 );

  #ifdef DEBUG
    int i = count, k = 0;
    printf("CheckPartitionStatus (%d): \n", count);
    for (i = 0; i < 3; i++) {
      fStack = (APEX_LONG_INTEGER *)&(Message_Stack[count - i][0]);
      for (k = 0; k < MSG_PARAMETERS; k++ ) {
        printf("%ld ", (long)fStack[k]);  
      }
      printf("\n");
    }
  #endif

  /* Part 3*/
  fStack = (APEX_LONG_INTEGER *)&(Message_Stack[count][0]);
  if ( fStack[0] != (APEX_LONG_INTEGER)3 ) return FALSE;
  if ( fStack[2] != (APEX_LONG_INTEGER)200000000 ) return FALSE;
  if ( fStack[3] != (APEX_LONG_INTEGER)60000000 ) return FALSE;
  if ( fStack[4] != (APEX_LONG_INTEGER)NORMAL_START ) return FALSE;


  /* Part 2*/
  fStack = (APEX_LONG_INTEGER *)&(Message_Stack[count - 1][0]); 
  if ( fStack[0] != (APEX_LONG_INTEGER)2 ) return FALSE;
  if ( fStack[1] != (APEX_LONG_INTEGER)NORMAL ) return FALSE;
  if ( fStack[2] != (APEX_LONG_INTEGER)200000000 ) return FALSE;
  if ( fStack[3] != (APEX_LONG_INTEGER)30000000 ) return FALSE;


  /* Part 1*/
  fStack = (APEX_LONG_INTEGER *)&(Message_Stack[count - 2][0]);
  if ( fStack[0] != (APEX_LONG_INTEGER)1 ) return FALSE;
  if ( fStack[1] != (APEX_LONG_INTEGER)NORMAL ) return FALSE;
  if ( fStack[2] != (APEX_LONG_INTEGER)200000000 ) return FALSE;
  if ( fStack[3] != (APEX_LONG_INTEGER)30000000 ) return FALSE;


  return TRUE;

}




void SetErrorBits(Matching_Error_Bits *ReturnStruct) {
  ReturnStruct->Cond3 = FALSE;
}


void SetNoErrorBits(Matching_Error_Bits *ReturnStruct, int count) {
  if (CheckPartitionStatus(count)) {
    ReturnStruct->Cond3 = TRUE;
  } else {
    printf("!Bad Partition Status!\n");
    ReturnStruct->Cond3 = FALSE;
  }
}





void MasterProcedure_T_API_MMS_0040_0031( TEST_RESULT_TYPE *TestResult ) {

	RETURN_CODE_TYPE RetCode = NO_ERROR;
  Matching_Error_Bits ReturnStruct;
	BOOLEAN FlagOK = FALSE;
	CheckSetModuleSchedule(2, &ReturnStruct, &RetCode);

	if ( ReturnStruct.Cond3 == TRUE ) {
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

  MasterProcedure_T_API_MMS_0040_0031( &TestResult );

  print_test_results (TestResult);
  print_testdef_result(TestResult);
  
  w_STOP_SELF();

}
