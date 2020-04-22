
#include "A653_MMS.h"





void CheckGetModuleScheduleId (
	SCHEDULE_NAME_TYPE SCHEDULE_NAME,
	SCHEDULE_ID_TYPE IN_SCHEDULE_ID,
	RETURN_CODE_TYPE IN_RETURN_CODE,
	Get_Schedule_Id_Errors *ReturnStruct,
	SCHEDULE_ID_TYPE *OUT_SCHEDULE_ID,
	RETURN_CODE_TYPE *OUT_RETURN_CODE) {


	#ifdef DEBUG
		printf("CheckGetModuleScheduleId\n");
	#endif


	w_GET_MODULE_SCHEDULE_ID(SCHEDULE_NAME, OUT_SCHEDULE_ID, OUT_RETURN_CODE);

	#ifdef DEBUG
		printf("w_GET_MODULE_SCHEDULE_ID %ld:%ld RC %ld:%ld\n", 
			(long)*OUT_SCHEDULE_ID,
			(long)IN_SCHEDULE_ID,
			(long)*OUT_RETURN_CODE,
			(long)IN_RETURN_CODE );
	#endif

	if (*OUT_SCHEDULE_ID != IN_SCHEDULE_ID) {
		ReturnStruct->SCHEDULE_ID = (unsigned)1;
	}

	if (*OUT_RETURN_CODE != IN_RETURN_CODE) {
		ReturnStruct->RETURN_CODE = (unsigned)1;
	}



}

