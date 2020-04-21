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





#include <a653.h>
#include <imaspex.h>
#include "Test_MMS_0060_0011.h"
#include "A653_MMS.h"

/* force size to be 12 */
/* implementation dependent */
int Number_of_Cycles = 0;
char TestName[] = "Test_MMS_0060_0011";
SYSTEM_TIME_TYPE Start_System_Time = 0;
char TestDefName[] = "2107";

SYSTEM_TIME_TYPE Current_Period_Start = 0;


void SetErrorBits(Matching_Error_Bits *ReturnStruct) {
  ReturnStruct->Cond7 = FALSE;
}


void SetNoErrorBits(
  SCHEDULE_ID_TYPE SCHEDULE_ID,
  Matching_Error_Bits *ReturnStruct,
  int count) {

    ReturnStruct->Cond7 = TRUE;

}


void EmptyReturnScruct ( Get_Schedule_Id_Errors *ReturnStruct ) {

  uint32_t *ptr = ReturnStruct;
  *ptr = 0x0;
}




void MasterProcedure_T_API_MMS_0060_0011( TEST_RESULT_TYPE *TestResult ) {

	RETURN_CODE_TYPE RetCode = NO_ERROR;
  SCHEDULE_ID_TYPE SchedId = 0;
  Get_Schedule_Id_Errors ReturnStruct;
	BOOLEAN FlagOK = FALSE;

  /*
  Call procedure CheckSetModuleSchedule with the following argument:
    - SCHEDULE_ID = 5

    INVALID_CONFIG as expected return code.
  */

  EmptyReturnStruct(&ReturnStruct);

  CheckGetModuleScheduleId( 
    "Test_Schedule",
    2,
    NO_ERROR,
    &ReturnStruct,
    &SchedId,
    &RetCode
    );

  uint32_t *aux;
  aux = (uint32_t *) &ReturnStruct;

  if (((*aux) ^ 0x0)){
    FlagOK = FALSE;
  } else {
    FlagOK = TRUE;
  }


	/* Fill return value */
  *TestResult = EvaluateResult( FlagOK );

}






void Initial () {

  TEST_RESULT_TYPE TestResult;
  RETURN_CODE_TYPE RetCode = NO_ERROR;
  QUEUING_PORT_ID_TYPE PortId;
  PROCESS_ATTRIBUTE_TYPE PROCESS_ATTRIBUTE;
  RETURN_CODE_TYPE RETURN_CODE;

  w_CREATE_QUEUING_PORT(
  	"Test_1_Dq", 
  	STR_SIZE,
  	4,
  	DESTINATION,
  	FIFO,
  	&PortId,
  	&RetCode);

  w_SET_PARTITION_MODE ( NORMAL, &RetCode );

  MasterProcedure_T_API_MMS_0060_0011( &TestResult );

  print_test_results (TestResult);
  print_testdef_result(TestResult);

  w_STOP_SELF();

}
