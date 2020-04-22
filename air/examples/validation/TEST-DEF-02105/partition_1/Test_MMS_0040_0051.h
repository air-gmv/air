/** 
 * @file Test_MMS_0040_0011.h
 * 
 *  COPYRIGHT (c) 2013
 *  GMV-SKYSOFT 
 *
 * @author Bruno Oliveira
 * @brief  Test-specific definitions and include files
 * 
 */



#ifndef _TEST_MMS_H_
#define _TEST_MMS_H_

#include "AVT_Types.h"
#include "AVT_independent.h"

#define PARTITION_3_PRESENT 1 /*Partition 3 Presnet in Schedule*/
#define STR_SIZE 64
#define STACK_SIZE 12 /* msg queue size (4) x 3 qports */
#define MAX_NUM_CYCLES 3
#define NUM_OF_SCHEDS 2
#define MSG_PARAMETERS 6


BOOLEAN CheckPartitionStatus( 
	SCHEDULE_ID_TYPE SCHEDULE_ID, 
	int count );

void MasterProcedure_T_API_MMS_0040_0011( TEST_RESULT_TYPE *TestResult );
void Initial ();

void SetErrorBits (Matching_Error_Bits *ReturnStruct);
void SetNoErrorBits (
	SCHEDULE_ID_TYPE SCHEDULE_ID,
	Matching_Error_Bits *ReturnStruct, 
	int count);

#endif
