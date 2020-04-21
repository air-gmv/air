#ifndef _A653_MMS_H_
#define _A653_MMS_H_

#include "AVT_independent.h"
#include "AVT_dependent.h"

#define STR_SIZE 64


typedef struct {
	unsigned UNUSED : 30;
	unsigned SCHEDULE_ID : 1;
	unsigned RETURN_CODE : 1;
	
} Get_Schedule_Id_Errors;

#endif
