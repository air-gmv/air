#include "A653_MMS.h"



void CheckGetModuleScheduleStatus( 
	SCHEDULE_STATUS_TYPE *SCHEDULE_STATUS,
	Get_Schedule_Errors *ReturnError,
	RETURN_CODE_TYPE *RETURN_CODE ) {

	BOOLEAN ErrorOccurred = FALSE;
	RETURN_CODE_TYPE RetCode = NO_ERROR;
	extern SYSTEM_TIME_TYPE Current_Period_Start;
	extern SYSTEM_TIME_TYPE Start_System_Time;

	#ifdef DEBUG
		printf("CheckGetModuleScheduleStatus\n");
	#endif



	w_GET_MODULE_SCHEDULE_STATUS(SCHEDULE_STATUS, RETURN_CODE);

	if (*RETURN_CODE != NO_ERROR) {
		ErrorOccurred = TRUE;
		ReturnError->GET_MODULE_SCHEDULE_STATUS = (unsigned)1;
	}

	#ifdef DEBUG
		printf("ErrorOccurred: %d\n", (int)ErrorOccurred);
		printf("TOLS: %ld, CS: %ld, NS: %ld\n", 
			(long)SCHEDULE_STATUS->TIME_OF_LAST_SCHEDULE_SWITCH,
			(long)SCHEDULE_STATUS->CURRENT_SCHEDULE,
			(long)SCHEDULE_STATUS->NEXT_SCHEDULE);
	#endif

	if (SCHEDULE_STATUS->TIME_OF_LAST_SCHEDULE_SWITCH != 
		Start_System_Time) {
		ErrorOccurred = TRUE;
		ReturnError->TIME_OF_LAST_SCHEDULE_SWITCH = (unsigned)1;
	}

	if (SCHEDULE_STATUS->CURRENT_SCHEDULE !=
		(SCHEDULE_ID_TYPE)1) {
		ErrorOccurred = TRUE;
		ReturnError->CURRENT_SCHEDULE = (unsigned)1;
	}

	if (SCHEDULE_STATUS->NEXT_SCHEDULE !=
		(SCHEDULE_ID_TYPE)1) {
		ErrorOccurred = TRUE;
		ReturnError->NEXT_SCHEDULE = (unsigned)1;
	}

	w_SET_MODULE_SCHEDULE( (SCHEDULE_ID_TYPE)2, &RetCode);
	if (RetCode != NO_ERROR) {
		ErrorOccurred = TRUE;
		ReturnError->SET_MODULE_SCHEDULE = (unsigned)1;
	}

	w_GET_MODULE_SCHEDULE_STATUS(SCHEDULE_STATUS, RETURN_CODE);

	#ifdef DEBUG
		printf("ErrorOccurred: %d\n", (int)ErrorOccurred);
		printf("TOLS: %ld, CS: %ld, NS: %ld\n", 
			(long)SCHEDULE_STATUS->TIME_OF_LAST_SCHEDULE_SWITCH,
			(long)SCHEDULE_STATUS->CURRENT_SCHEDULE,
			(long)SCHEDULE_STATUS->NEXT_SCHEDULE);
	#endif

	if (*RETURN_CODE != NO_ERROR) {
		ErrorOccurred = TRUE;
		ReturnError->GET_MODULE_SCHEDULE_STATUS = (unsigned)1;
	}

	if (SCHEDULE_STATUS->TIME_OF_LAST_SCHEDULE_SWITCH != 
		Start_System_Time) {
		ErrorOccurred = TRUE;
		ReturnError->TIME_OF_LAST_SCHEDULE_SWITCH = (unsigned)1;
	}

	if (SCHEDULE_STATUS->CURRENT_SCHEDULE !=
		(SCHEDULE_ID_TYPE)1) {
		ErrorOccurred = TRUE;
		ReturnError->CURRENT_SCHEDULE = (unsigned)1;
	}

	if (SCHEDULE_STATUS->NEXT_SCHEDULE !=
		(SCHEDULE_ID_TYPE)2) {
		ErrorOccurred = TRUE;
		ReturnError->NEXT_SCHEDULE = (unsigned)1;
	}




	w_TIMED_WAIT( 200000000ll, &RetCode);
	if (RetCode != NO_ERROR) {
		ErrorOccurred = TRUE;
		ReturnError->TIMED_WAIT = (unsigned)1;
	}

	/*
	//Cannot be done here because Master partition
	//is not the first in the MTF

	w_GET_TIME(&Current_Period_Start, &RetCode);
	if (RetCode != NO_ERROR) {
		ErrorOccurred = TRUE;
		ReturnError->GET_TIME = TRUE;
	}
	*/





	w_GET_MODULE_SCHEDULE_STATUS(SCHEDULE_STATUS, RETURN_CODE);

	#ifdef DEBUG
		printf("ErrorOccurred: %d\n", (int)ErrorOccurred);
		printf("TOLS: %ld, CS: %ld, NS: %ld\n", 
			(long)SCHEDULE_STATUS->TIME_OF_LAST_SCHEDULE_SWITCH,
			(long)SCHEDULE_STATUS->CURRENT_SCHEDULE,
			(long)SCHEDULE_STATUS->NEXT_SCHEDULE);
	#endif

	if (*RETURN_CODE != NO_ERROR) {
		ErrorOccurred = TRUE;
		ReturnError->GET_MODULE_SCHEDULE_STATUS = (unsigned)1;
	}
	if (SCHEDULE_STATUS->TIME_OF_LAST_SCHEDULE_SWITCH != 
		Current_Period_Start) {
		ErrorOccurred = TRUE;
		ReturnError->TIME_OF_LAST_SCHEDULE_SWITCH = (unsigned)1;
		printf("%ld\n", (long)Current_Period_Start);
	}

	if (SCHEDULE_STATUS->CURRENT_SCHEDULE !=
		(SCHEDULE_ID_TYPE)2) {
		ErrorOccurred = TRUE;
		ReturnError->CURRENT_SCHEDULE = (unsigned)1;
	}

	if (SCHEDULE_STATUS->NEXT_SCHEDULE !=
		(SCHEDULE_ID_TYPE)2) {
		ErrorOccurred = TRUE;
		ReturnError->NEXT_SCHEDULE = (unsigned)1;
	}



}