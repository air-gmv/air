/** 
 * @file AVT_Types.h
 * 
 *  COPYRIGHT (c) 2013
 *  GMV-SKYSOFT 
 *
 * @author Bruno Oliveira
 * @brief  Essential type definitions, mostly inherited from AVT,
 *		or following the same primitives for future re-implementation
 * 
 */



#ifndef _AVT_TYPES_H
#define _AVT_TYPES_H

#include <rtems.h>


/*Strucutre defining Requirement condition validations referenced 
		in A653-Part3 for MMS
*/

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
