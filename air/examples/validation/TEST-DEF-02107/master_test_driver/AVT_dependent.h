/** 
 * @file AVT_dependent.h
 * 
 *  COPYRIGHT (c) 2013
 *  GMV-SKYSOFT 
 *
 * @author Bruno Oliveira
 * @brief  AVT function prototypes
 * @note Although target-dependent, the functions contained in this 
 *		file should be compatible with other AVT implementations, i.e., 
 *		this functions should be replaceable by equivalent implementations 
 * 
 */


#ifndef _AVT_DEPENDENT_H_
#define _AVT_DEPENDENT_H_


#define DEBUG



#include <rtems.h>
#include <imaspex.h>
#include "AVT_independent.h"


void w_TIMED_WAIT ( APEX_LONG_INTEGER NanoSeconds , RETURN_CODE_TYPE *RetCode);
void w_STOP_SELF ( void );

void w_CREATE_PROCESS( 
	PROCESS_ATTRIBUTE_TYPE *ATTRIBUTES, /*in*/
	PROCESS_ID_TYPE *PROCESS_ID, /*out*/
	RETURN_CODE_TYPE *RETURN_CODE /*out*/ 
	);

	void CalcPeriodStart (void);

#endif