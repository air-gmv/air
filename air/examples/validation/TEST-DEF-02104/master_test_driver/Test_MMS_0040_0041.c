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



#include "Test_MMS_0040_0041.h"
#include "AVT_MMS.h"


BOOLEAN Part3InSched[NUM_OF_SCHEDS] = {TRUE, FALSE};

/* force size to be 12 */
/* implementation dependent */
APEX_LONG_INTEGER Correct_Order[STACK_SIZE] = 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int Number_of_Cycles = 0;
char TestName[] = "Test_MMS_0040_0041";
char TestDefName[] = "2104";


extern char *Message_Addr;


void SetErrorBits(Matching_Error_Bits *ReturnStruct) {
  ReturnStruct->Cond4 = FALSE;
}


void SetNoErrorBits(
  SCHEDULE_ID_TYPE SCHEDULE_ID,
  Matching_Error_Bits *ReturnStruct,
  int count) {

  if (CheckPartitionStatus(SCHEDULE_ID, count)) {
    ReturnStruct->Cond4 = TRUE;
  } else {
    printf("!Bad Partition Status!\n");
    ReturnStruct->Cond4 = FALSE;
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
  int i = count, k = 0;
  APEX_LONG_INTEGER *fStack = NULL;
  APEX_LONG_INTEGER LastTime, PreviousTime = 0;


  /*------------------------------------------------------------------
  -------------------------------------------------------------------*/
  switch (SCHEDULE_ID) {
    case (SCHEDULE_ID_TYPE)1:
        /* Search The first Message from partition 3 */
      do {
        count--;
        fStack = (APEX_LONG_INTEGER *)&(Message_Stack[count][0]);
        } while ( fStack[0] != (APEX_LONG_INTEGER)3 );

      #ifdef DEBUG
        printf("CheckPartitionStatus (%ld)(%d): \n", 
          (long)SCHEDULE_ID,
          count);
        for (i = 0; i < 6; i++) {
          fStack = (APEX_LONG_INTEGER *)&(Message_Stack[count - i][0]);
          for (k = 0; k < MSG_PARAMETERS; k++ ) {
            printf("%ld ", (long)fStack[k]);  
          }
          printf("\n");
        }
      #endif



      /* Part 3, Sched 1*/
      fStack = (APEX_LONG_INTEGER *)&(Message_Stack[count][0]);
      if ( fStack[0] != (APEX_LONG_INTEGER)3 ) return FALSE;
      if ( fStack[2] != (APEX_LONG_INTEGER)200000000 ) return FALSE;
      if ( fStack[3] != (APEX_LONG_INTEGER)60000000 ) return FALSE;
      if ( fStack[4] != (APEX_LONG_INTEGER)NORMAL_START ) return FALSE;
      LastTime = (APEX_LONG_INTEGER)fStack[5];


      /* Part 2, Sched 1*/
      fStack = (APEX_LONG_INTEGER *)&(Message_Stack[count - 1][0]); 
      if ( fStack[0] != (APEX_LONG_INTEGER)2 ) return FALSE;
      if ( fStack[1] != (APEX_LONG_INTEGER)NORMAL ) return FALSE;
      if ( fStack[2] != (APEX_LONG_INTEGER)200000000 ) return FALSE;
      if ( fStack[3] != (APEX_LONG_INTEGER)30000000 ) return FALSE;


      /* Part 1, Sched 1*/
      fStack = (APEX_LONG_INTEGER *)&(Message_Stack[count - 2][0]);
      if ( fStack[0] != (APEX_LONG_INTEGER)1 ) return FALSE;
      if ( fStack[1] != (APEX_LONG_INTEGER)NORMAL ) return FALSE;
      if ( fStack[2] != (APEX_LONG_INTEGER)200000000 ) return FALSE;
      if ( fStack[3] != (APEX_LONG_INTEGER)30000000 ) return FALSE;

      /* Part 3, left from the initial schedule*/
      fStack = (APEX_LONG_INTEGER *)&(Message_Stack[count - 3][0]);
      if ( fStack[0] != (APEX_LONG_INTEGER)3 ) return FALSE;
      if ( fStack[2] != (APEX_LONG_INTEGER)200000000 ) return FALSE;
      if ( fStack[3] != (APEX_LONG_INTEGER)60000000 ) return FALSE;
      if ( fStack[4] != (APEX_LONG_INTEGER)NORMAL_START ) return FALSE;
      PreviousTime = (APEX_LONG_INTEGER)fStack[5];

      /* Part 2, Sched 2*/
      fStack = (APEX_LONG_INTEGER *)&(Message_Stack[count - 4][0]); 
      if ( fStack[0] != (APEX_LONG_INTEGER)2 ) return FALSE;
      if ( fStack[1] != (APEX_LONG_INTEGER)NORMAL ) return FALSE;
      if ( fStack[2] != (APEX_LONG_INTEGER)200000000 ) return FALSE;
      if ( fStack[3] != (APEX_LONG_INTEGER)50000000 ) return FALSE;


      /* Part 1, Sched 2*/
      fStack = (APEX_LONG_INTEGER *)&(Message_Stack[count - 5][0]);
      if ( fStack[0] != (APEX_LONG_INTEGER)1 ) return FALSE;
      if ( fStack[1] != (APEX_LONG_INTEGER)NORMAL ) return FALSE;
      if ( fStack[2] != (APEX_LONG_INTEGER)200000000 ) return FALSE;
      if ( fStack[3] != (APEX_LONG_INTEGER)50000000 ) return FALSE;


      if (LastTime < PreviousTime) return FALSE;

      return TRUE;

    break;


    case (SCHEDULE_ID_TYPE)2:
        /* Search The first Message from partition 2 */
      do {
        count--;
        fStack = (APEX_LONG_INTEGER *)&(Message_Stack[count][0]);
        } while ( fStack[0] != (APEX_LONG_INTEGER)2 );

      #ifdef DEBUG

        printf("CheckPartitionStatus (%ld)(%d): \n", 
          (long)SCHEDULE_ID,
          count);
        for (i = 0; i < 2; i++) {
          fStack = (APEX_LONG_INTEGER *)&(Message_Stack[count - i][0]);
          for (k = 0; k < MSG_PARAMETERS; k++ ) {
            printf("%ld ", (long)fStack[k]);  
          }
          printf("\n");
        }
      #endif

      /* Part 2, Sched 2*/
      fStack = (APEX_LONG_INTEGER *)&(Message_Stack[count][0]); 
      if ( fStack[0] != (APEX_LONG_INTEGER)2 ) return FALSE;
      if ( fStack[1] != (APEX_LONG_INTEGER)NORMAL ) return FALSE;
      if ( fStack[2] != (APEX_LONG_INTEGER)200000000 ) return FALSE;
      if ( fStack[3] != (APEX_LONG_INTEGER)50000000 ) return FALSE;

      /* Part 1, Sched 2*/
      fStack = (APEX_LONG_INTEGER *)&(Message_Stack[count - 1][0]);
      if ( fStack[0] != (APEX_LONG_INTEGER)1 ) return FALSE;
      if ( fStack[1] != (APEX_LONG_INTEGER)NORMAL ) return FALSE;
      if ( fStack[2] != (APEX_LONG_INTEGER)200000000 ) return FALSE;
      if ( fStack[3] != (APEX_LONG_INTEGER)50000000 ) return FALSE;


      return TRUE;

    break;


    /*------------------------------------------------------------------
    -------------------------------------------------------------------*/

    default:
      return FALSE;
    break;

  }

}


void EmptyOrderVector ( int value ) {

  int k;
  for (k = 0; k < STACK_SIZE; k++) {
    Correct_Order[k] = (APEX_LONG_INTEGER)value;
  }

}




void MasterProcedure_T_API_MMS_0040_0041( TEST_RESULT_TYPE *TestResult ) {

	RETURN_CODE_TYPE RetCode = NO_ERROR;
  Matching_Error_Bits ReturnStruct;
	BOOLEAN FlagOK = FALSE;
  extern BOOLEAN Test_Not_Over;
  extern APEX_INTEGER CycleCounter;

  /* Assign Correct Partition ID's order to be received via 
  queuing message */
  EmptyOrderVector(-1);
  Test_Not_Over = TRUE;
  CycleCounter = 0;
  Correct_Order[0] = 1;Correct_Order[1] = 2;Correct_Order[2] = 3;
  Correct_Order[3] = 1;Correct_Order[4] = 2;
  Correct_Order[5] = 1;Correct_Order[6] = 2;
  Number_of_Cycles = 5;
  /*Set new Module Schedule and check correct partition
  execution order and status */
	CheckSetModuleSchedule(2, &ReturnStruct, &RetCode);

  
  if ( (ReturnStruct.Cond4 == TRUE) && 
        (RetCode == NO_ERROR) ) {
    /* Assign Correct Partition ID's order to be received via 
    queuing message */
    EmptyOrderVector(-1);
    Test_Not_Over = TRUE;
    CycleCounter = 0;
    Correct_Order[0] = 1;Correct_Order[1] = 2;Correct_Order[2] = 3;
    Correct_Order[3] = 1;Correct_Order[4] = 2;Correct_Order[5] = 3;
    Number_of_Cycles = 3;
    /*Set new Module Schedule and check correct partition
    execution order and status */
    CheckSetModuleSchedule(1, &ReturnStruct, &RetCode);
  }

	if ( (ReturnStruct.Cond4 == TRUE) && 
        (RetCode == NO_ERROR) ) {
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

  MasterProcedure_T_API_MMS_0040_0041( &TestResult );

  print_test_results (TestResult);
  print_testdef_result(TestResult);

  w_STOP_SELF();

}
