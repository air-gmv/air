
#ifndef _AVT_TYPES_H
#define _AVT_TYPES_H

#include <rtems.h>

typedef struct Matching_Error_Bits_s{

	unsigned int Cond1; 
	unsigned int Cond2; 
	unsigned int Cond3; 
	unsigned int Cond4; 
	unsigned int Cond5; 
	unsigned int Cond6; 
	unsigned int Cond7; 
	unsigned int Cond8;
	
} Matching_Error_Bits;



typedef enum 
{
	NOT_PERFORMED = 0,
	FAILED = 1,
	SUCCESS = 2
} TEST_RESULT_TYPE;

typedef rtems_boolean BOOLEAN;


#endif

