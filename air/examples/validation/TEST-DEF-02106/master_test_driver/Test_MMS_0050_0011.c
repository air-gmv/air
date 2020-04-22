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
#include "Test_MMS_0050_0011.h"
#include "A653_MMS.h"

/* force size to be 12 */
/* implementation dependent */
int Number_of_Cycles = 0;
char TestName[] = "Test_MMS_0050_0011";
char TestDefName[] = "2106";

SYSTEM_TIME_TYPE Start_System_Time = 0;

SYSTEM_TIME_TYPE Current_Period_Start = 0;




void SetErrorBits(Matching_Error_Bits *ReturnStruct) {
  ReturnStruct->Cond6 = FALSE;
}


void SetNoErrorBits(
  SCHEDULE_ID_TYPE SCHEDULE_ID,
  Matching_Error_Bits *ReturnStruct,
  int count) {

    ReturnStruct->Cond6 = TRUE;

}







void EmptySchedStatus(SCHEDULE_STATUS_TYPE *SCHEDULE_STATUS) {
  SCHEDULE_STATUS->TIME_OF_LAST_SCHEDULE_SWITCH = 
    (SYSTEM_TIME_TYPE)0;
  SCHEDULE_STATUS->NEXT_SCHEDULE = (SCHEDULE_ID_TYPE)0;
  SCHEDULE_STATUS->CURRENT_SCHEDULE = (SCHEDULE_ID_TYPE)0;
}



void MasterProcedure_T_API_MMS_0050_0011( TEST_RESULT_TYPE *TestResult ) {

	RETURN_CODE_TYPE RetCode = NO_ERROR;
  Get_Schedule_Errors ReturnStruct;
	BOOLEAN FlagOK = FALSE;
  SCHEDULE_STATUS_TYPE SchedStatus;


  /*
  Call procedure CheckSetModuleSchedule with the following argument:
    - SCHEDULE_ID = 5

    INVALID_CONFIG as expected return code.
  */

  EmptyReturnStruct(&ReturnStruct);
  EmptySchedStatus(&SchedStatus);

  CheckGetModuleScheduleStatus( 
    &SchedStatus,
    &ReturnStruct,
    &RetCode) ;

  uint32_t *aux;
  aux = (uint32_t *) &ReturnStruct;

  if (((*aux) ^ 0x0) && (RetCode == NO_ERROR) ){
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


  PROCESS_ID_TYPE PROCESS_ID;

  char pName[5] = "CPST";


  
  PROCESS_ATTRIBUTE.PERIOD = (SYSTEM_TIME_TYPE) 200000000ll;
  PROCESS_ATTRIBUTE.TIME_CAPACITY = (SYSTEM_TIME_TYPE) 100000000ll;
  PROCESS_ATTRIBUTE.ENTRY_POINT = (SYSTEM_ADDRESS_TYPE) CalcPeriodStart;
  PROCESS_ATTRIBUTE.STACK_SIZE = (STACK_SIZE_TYPE) 4096;
  PROCESS_ATTRIBUTE.BASE_PRIORITY = (PRIORITY_TYPE) 10;
  PROCESS_ATTRIBUTE.DEADLINE = (DEADLINE_TYPE) SOFT;
  strncpy((char *)&(PROCESS_ATTRIBUTE.NAME), pName, 4);


  w_CREATE_PROCESS(
    &PROCESS_ATTRIBUTE,
    &PROCESS_ID,
    &RETURN_CODE);





  w_SET_PARTITION_MODE ( NORMAL, &RetCode );

  MasterProcedure_T_API_MMS_0050_0011( &TestResult );

  print_test_results (TestResult);
  print_testdef_result(TestResult);

  w_STOP_SELF();

}
