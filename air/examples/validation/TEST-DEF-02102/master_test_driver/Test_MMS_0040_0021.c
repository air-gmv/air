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



#include "Test_MMS_0040_0021.h"

/* force size to be 12 */
/* implementation dependent */
APEX_LONG_INTEGER Correct_Order[12] = 
	{1, 2, 2, 1, 2, 1, 2, 1, -1, -1, -1, -1};
char TestName[] = "Test_MMS_0040_0021";
char TestDefName[] = "2102";


extern char **Message_Stack; 
extern char *Message_Addr;


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: MasterProcedure_T_API_MMS_0040_00011                | *
 * |                                                                    | *
 * | DESCRIPTION:  Master test procedure. Callable from AVT             | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   16/09/2013                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                         Input  Output  Description     | *
 * | ENUM TEST_RESULT_TYPE *TestResult           X     Test result      | *
 * |                                                   SUCCESS or FAILED| *
 * | RETURN VALUE:  void                                                | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */

void MasterProcedure_T_API_MMS_0040_0011( TEST_RESULT_TYPE *TestResult ) {

	RETURN_CODE_TYPE RetCode = NO_ERROR;
  Matching_Error_Bits ReturnStruct;
	BOOLEAN FlagOK = FALSE;
	CheckSetModuleSchedule(2, &ReturnStruct, &RetCode);

	if ( ReturnStruct.Cond1 == TRUE ) {
		FlagOK = TRUE;
	} else {
    FlagOK = FALSE;
  }


	/* Fill return value */
  *TestResult = EvaluateResult( FlagOK );

}




/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: Initial                                             | *
 * |                                                                    | *
 * | DESCRIPTION: Initializes necessary ports, puts partition in NORMAL | *
 * |      operating mode. Calls the test master procedure               | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   16/09/2013                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                         Input  Output  Description     | *
 * | ENUM TEST_RESULT_TYPE *TestResult           X     Test result      | *
 * |                                                   SUCCESS or FAILED| *
 * | RETURN VALUE:  void                                                | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */



void
Initial () {

  TEST_RESULT_TYPE TestResult;
  RETURN_CODE_TYPE RetCode = NO_ERROR;
  QUEUING_PORT_ID_TYPE PortId;

  /* Create Queuing Ports */

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

  /*Set Partition Mode to normal */

  w_SET_PARTITION_MODE ( NORMAL, &RetCode );

  /*Call Partition Master procedure */

  MasterProcedure_T_API_MMS_0040_0011( &TestResult );

  /*Print Test Results based on the output from the MasterProcedure */

  print_test_results (TestResult);
  print_testdef_result(TestResult);

  /* Stop Execution */

  w_STOP_SELF();

}
