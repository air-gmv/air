/** 
 * @file AVT_dependent.h
 * 
 *  COPYRIGHT (c) 2013
 *  GMV-SKYSOFT 
 *
 * @author Bruno Oliveira
 * @brief  AVT function prototypes
  * @note The functions contained in this file should be compatible with 
 *    other AVT implementations, i.e., this functions should substitute 
 *		other implementations 
 * 
 */


#ifndef _AVT_DEPENDENT_H_
#define _AVT_DEPENDENT_H_


#define DEBUG



#include <rtems.h>
#include <imaspex.h>
#include "AVT_independent.h"


void w_TIMED_WAIT ( APEX_LONG_INTEGER NanoSeconds , RETURN_CODE_TYPE *RetCode);
void w_STOP_SELF( void );

#endif
