/** 
 * @file AVT_MMS.h
 * 
 *  COPYRIGHT (c) 2013
 *  GMV-SKYSOFT 
 *
 * @author Bruno Oliveira
 * @brief  A653-P3 MMS Macro function prototypes along with 
 * 		essential includes
 * 
 */


#ifndef _AVT_MMS_H_
#define _AVT_MMS_H_

#include <a653.h>
#include "AVT_Types.h"
#include "Test_MMS_0040_0051.h"
#include "AVT_dependent.h"
#include "AVT_independent.h"


/* Function Prototypes */

BOOLEAN CheckMessageOrder( int count );

void CheckSetModuleSchedule ( 
	SCHEDULE_ID_TYPE SCHEDULE_ID,
	Matching_Error_Bits *ReturnStruct, 
	RETURN_CODE_TYPE *RETURN_CODE
	);


BOOLEAN isPart3InSched( void );



#endif
