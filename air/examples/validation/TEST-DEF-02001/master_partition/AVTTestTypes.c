/*------------------------------------------------------------------------+
 * +xxxxxxxxxxxxxxxxxxxxxx SKYSOFT PORTUGAL SA xxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * | LEGAL NOTICE: The source code contained herein is not intended to  | *
 * | allow the development of source code or software for use or        | *
 * | distribution. This file contains SKYSOFT PORTUGAL, SA proprietary  | *
 * | information and is supplied on the condition that it shall solely  | *
 * | be used within the terms of the AVT License Agreement. Except as   | *
 * | otherwise provided thereof, you may not,without prior written      | *
 * | permission from SKYSOFT PORTUGAL, S.A. provide, or otherwise       | *
 * | disclose information regarding any discovered bugs, errors,        | *
 * | architecture issues or problems with the Source Code or Compiled   | *
 * | Code to any other party.                                           | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *                                                                        *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * | FILE: AVTTestTypes.c                                               | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: AVTTESTTYPES.C                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): Test_GEN_010                                | *
 * |                        Test_PART_010                               | *
 * |                        Test_PROC_010                               | *
 * |                        Test_INTER_010                              | *
 * |                        Test_INTRA_010                              | *
 * |                        Test_HMON_010                               | *
 * |                                                                    | *
 * | DESCRIPTION: DATA TYPES VERIFICATION                               | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    10/11/2002                                                | *
 * |                                                                    | *
 * | CVR COVERED: To see in each one of the functions                   | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/
#include "AVTTestTypes.h"



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_GEN_010                                       | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   27/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-GEN-0010                                          | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_GEN_010( APEX_BYTE value )
{
	value = 0;
	value = APEX_BYTE_MAX;
	value = APEX_BYTE_MAX + 1;
	value = APEX_BYTE_MIN;
	value = APEX_BYTE_MIN - 1;
	return ( TRUE );
}  /* Test_GEN_010  */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_GEN_020                                       | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   27/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-GEN-0020                                          | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_GEN_020( APEX_INTEGER value )
{
	value = 0;
	value = APEX_INTEGER_MAX;
	value = APEX_INTEGER_MAX + 1;
	value = APEX_INTEGER_MIN;
	value = APEX_INTEGER_MIN - 1;
	return ( TRUE );
}  /* Test_GEN_020  */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_GEN_030                                       | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   27/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-GEN-0030                                          | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_GEN_030( APEX_UNSIGNED value )
{
	value = 123;
	value = APEX_UNSIGNED_MAX;
	value = APEX_UNSIGNED_MAX + 1;	
	value = 0;
	value = 1;	
	return ( TRUE );
}  /* Test_GEN_030  */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_GEN_040                                       | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   27/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-GEN-0040                                          | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_GEN_040( APEX_LONG_INTEGER value )
{
	value = 0;
	value = APEX_LONG_INTEGER_MAX;
	value = APEX_LONG_INTEGER_MAX + 1;
	value = APEX_LONG_INTEGER_MIN;
	value = APEX_LONG_INTEGER_MIN - 1;
	return ( TRUE );
}  /* Test_GEN_040  */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_GEN_050                                       | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   27/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-GEN-0050                                          | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_GEN_050( NAME_TYPE value )
{
	/*this space must be filled with implementation dependent test code*/
	APEX_UNSIGNED i = 0;
	
	for ( i = 0; i < MAX_NAME_LENGTH; i ++ )
	{
		value[i] = i;
	}
	return ( TRUE );
}  /* Test_GEN_050  */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_GEN_060                                       | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   10/11/2002                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-GEN-0060, API-GEN-0060                            | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_GEN_060( RETURN_CODE_TYPE value )
{
  /* RETURN_CODE_TYPE */
  value = NO_ERROR;
  value = NO_ACTION;
  value = NOT_AVAILABLE;
  value = INVALID_PARAM;
  value = INVALID_CONFIG;
  value = INVALID_MODE;
  value = TIMED_OUT;
  
  return ( TRUE );
}  /* Test_GEN_060  */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_GEN_070                                       | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   27/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-GEN-0070                                          | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_GEN_070( SYSTEM_ADDRESS_TYPE value )
{
	value = NULL;
	/*this space must be filled with implementation dependent test code*/
	value = Test_GEN_070;
	/*no values put for maximum or minimum pointer*/
	/******************************************************************/
	return ( TRUE );
}  /* Test_GEN_070  */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_GEN_080                                       | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   27/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-GEN-0080                                          | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_GEN_080( SYSTEM_TIME_TYPE time )
{
	return ( Test_GEN_040 (time) );
}  /* Test_GEN_080  */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_GEN_090                                       | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   27/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-GEN-0090                                          | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_GEN_090( MESSAGE_ADDR_TYPE address )
{
	char temp[20];
	
	address = NULL;
	/*this space must be filled with implementation dependent test code*/
	address = (unsigned char *)temp;
	/*no values put for maximum or minimum pointer*/
	/******************************************************************/
	return ( TRUE );
}  /* Test_GEN_090  */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_GEN_100                                       | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   27/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-GEN-0100                                          | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_GEN_100( MESSAGE_SIZE_TYPE size )
{
	return ( Test_GEN_020 (size) );
}  /* Test_GEN_100  */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_PART_010                                      | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   10/11/2002                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-PART-0180                                         | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_PART_010 ( OPERATING_MODE_TYPE OperType )
{
  OperType = IDLE;
  OperType = COLD_START;
  OperType = WARM_START;
  OperType = NORMAL;
  
  return ( TRUE );  
}  /* Test_PART_010 */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_PART_020                                      | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-PART-0190                                         | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_PART_020 ( PARTITION_ID_TYPE  PrtType )
{
  return ( Test_GEN_020 ( PrtType ) );
}  /* Test_PART_020  */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_PART_030                                      | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-PART-0200                                         | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_PART_030 ( LOCK_LEVEL_TYPE Level )
{
  return ( Test_GEN_020 ( Level ) );
}  /* Test_PART_030  */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_PART_040                                      | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-PART-0205                                         | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_PART_040 ( START_CONDITION_TYPE condition )
{
	condition = NORMAL_START;
	condition = PARTITION_RESTART;
	//condition = HD_MODULE_RESTART;
	condition = HM_PARTITION_RESTART;
	return ( TRUE );
}  /* Test_PART_040  */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_PART_050                                      | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-PART-0210                                         | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_PART_050()
{
   PARTITION_STATUS_TYPE  PrtStatus;
	
	return ( Test_PART_020 ( PrtStatus.IDENTIFIER ) &&
			   Test_GEN_080  ( PrtStatus.PERIOD ) &&
			   Test_GEN_080  ( PrtStatus.DURATION ) &&
/*name was changed in ARINC 653 Skysoft simulator due to conflicts with system*/			
			   Test_PART_030 ( PrtStatus.LOCK_LEVEL ) &&
			   Test_PART_010 ( PrtStatus.OPERATING_MODE ) &&
			   Test_PART_040 ( PrtStatus.START_CONDITION ) );
}  /* Test_PART_050  */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_PROC_010                                      | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-PROC-0110                                         | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_PROC_010 ( PROCESS_ID_TYPE Level ) 
{
  return ( Test_GEN_020 ( Level ) );
}  /* Test_PROC_010  */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_PROC_020                                      | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-PROC-0120                                         | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_PROC_020 ( PROCESS_NAME_TYPE Name )
{
  return ( Test_GEN_050 ( Name ) );
}  /* Test_PROC_020  */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_PROC_030                                      | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-PROC-0130                                         | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_PROC_030 ( PRIORITY_TYPE Level )
{
  return ( Test_GEN_020 ( Level ) );
}  /* Test_PROC_030  */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_PROC_040                                      | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-PROC-0140                                         | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_PROC_040 ( STACK_SIZE_TYPE Level )
{
  return ( Test_GEN_030 ( Level ) );
}  /* Test_PROC_040  */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_PROC_050                                      | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-PROC-0150                                         | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_PROC_050 ( LOCK_LEVEL_TYPE Level )
{
  return ( Test_GEN_020 ( Level ) );
}  /* Test_PROC_050  */




/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_INTER_010                                      | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0370                                         | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_INTER_010 ( PORT_DIRECTION_TYPE PortDir )
{
   PortDir = SOURCE;
   PortDir = DESTINATION;
	return ( TRUE );
}  /* Test_INTER_010   */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_INTER_020                                     | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0400                                        | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_INTER_020 ( )
{
	SAMPLING_PORT_ID_TYPE PortId;
	
	return ( Test_GEN_020 ( PortId ) );
}  /* Test_INTER_020   */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_INTER_030                                     | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0410                                        | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_INTER_030 ( )
{
	SAMPLING_PORT_NAME_TYPE PortName;
	
	return ( Test_GEN_050 ( PortName ) );
}  /* Test_INTER_030   */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_INTER_040                                     | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0420                                        | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_INTER_040 ( 	VALIDITY_TYPE validity )
{
	validity = VALID;
	validity = INVALID;
	return ( TRUE );
}  /* Test_INTER_040   */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_INTER_050                                     | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0430                                        | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_INTER_050 ( )
{
  SAMPLING_PORT_STATUS_TYPE  SPStatus;
  
  return (  Test_GEN_100   ( SPStatus.MAX_MESSAGE_SIZE ) &&
            Test_INTER_010 ( SPStatus.PORT_DIRECTION ) &&
            Test_GEN_080   ( SPStatus.REFRESH_PERIOD ) &&
            Test_INTER_040 ( SPStatus.LAST_MSG_VALIDITY ));

}  /* Test_INTER_050   */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_INTER_060                                     | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0440                                        | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_INTER_060 ( )
{
	QUEUING_PORT_ID_TYPE PortId;
	
	return ( Test_GEN_020 ( PortId ) );
}  /* Test_INTER_060   */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_INTER_070                                     | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0450                                        | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_INTER_070 ( )
{
   QUEUING_PORT_NAME_TYPE PortName;
	
	return ( Test_GEN_050 ( PortName ) );
}  /* Test_INTER_070   */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_INTER_080                                     | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0470                                        | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_INTER_080 ( )
{
  QUEUING_DISCIPLINE_TYPE QPDiscipline;
	
  QPDiscipline = FIFO;
  QPDiscipline = PRIORITY;
  
  return ( TRUE );
}  /* Test_INTER_080   */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_INTER_090                                     | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0480                                        | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_INTER_090 ( MESSAGE_RANGE_TYPE MsgRange )
{
	return ( Test_GEN_020 ( MsgRange ) );
}  /* Test_INTER_090   */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_INTER_100                                     | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0490                                        | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_INTER_100 ( WAITING_RANGE_TYPE WaitRange )
{
	return ( Test_GEN_020 ( WaitRange ) );
}  /* Test_INTER_100   */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_INTER_110                                     | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/10/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0500                                        | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_INTER_110 ( )
{
  QUEUING_PORT_STATUS_TYPE  QPStatus;
  
  return (  Test_INTER_090 ( QPStatus.NB_MESSAGE ) &&
            Test_INTER_090 ( QPStatus.MAX_NB_MESSAGE ) &&
            Test_GEN_100   ( QPStatus.MAX_MESSAGE_SIZE ) &&
            Test_INTER_010 ( QPStatus.PORT_DIRECTION ) &&
		      Test_INTER_100 ( QPStatus.WAITING_PROCESSES ) );
}  /* Test_INTER_110   */



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_HMON_010                                     | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   02/11/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-HMON-0023                                         | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_HMON_010 ( ERROR_MESSAGE_TYPE  ErrorMessage )
{
	APEX_UNSIGNED       i = 0;
	
	for ( i = 0; i < MAX_ERROR_MESSAGE_SIZE; i ++ )
	{
		ErrorMessage[i] = 100;
	}
	return ( TRUE );
}  /* Test_HMON_010 */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_HMON_020                                     | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   02/11/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-HMON-0027                                         | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_HMON_020 ( ERROR_MESSAGE_SIZE_TYPE  ErrorMessageSize )
{
	return ( Test_GEN_020 ( ErrorMessageSize ) );
}  /* Test_HMON_020 */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_HMON_030                                     | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   02/11/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-HMON-0030                                         | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_HMON_030 ( ERROR_CODE_TYPE  ErrorCode )
{
	ErrorCode = DEADLINE_MISSED;
	ErrorCode = APPLICATION_ERROR;
	ErrorCode = NUMERIC_ERROR;
	ErrorCode = ILLEGAL_REQUEST;
	ErrorCode = STACK_OVERFLOW;
	ErrorCode = MEMORY_VIOLATION;
	ErrorCode = HARDWARE_FAULT;
	ErrorCode = POWER_FAIL;
	
	return ( TRUE );
}  /* Test_HMON_030 */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  Test_HMON_040                                     | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   02/11/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: API-HMON-0040                                         | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN Test_HMON_040 ( )
{
  ERROR_STATUS_TYPE  ErrorStatus;
	
  return (  Test_HMON_030 ( ErrorStatus.ERROR_CODE ) &&
		      Test_HMON_010 ( ErrorStatus.MESSAGE ) &&                
		      Test_HMON_020 ( ErrorStatus.LENGTH ) &&                
		     // Test_PROC_010 ( ErrorStatus.FAILED_PROCESS_ID ) &&
		      Test_GEN_070  ( ErrorStatus.FAILED_ADDRESS ) );
}  /* Test_HMON_040 */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:  General_API_Test                                     | *
 * |                                                                    | *
 * | DESCRIPTION: THE FOLLOWING TEST ENSURES THAT THE EXISTING DATA     | *
 * | TYPES CONCERNING THE GENERIC API SERVICES COMPILE SEAMLESSLY WITHIN| *
 * | THE TEST FRAMEWORK.                                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   02/11/2004                                                 | *
 * |                                                                    | *
 * | CVR COVERED: All the functions on this file will be called either  | *
 * /              directly or not                                       | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
BOOLEAN General_API_Test ( )
{
  ERROR_STATUS_TYPE  ErrorStatus;
	
  return (  Test_PART_050 ( ) &&
		      Test_INTER_020 ( ) && 
		      Test_INTER_030 ( ) && 
		      Test_INTER_050 ( ) &&
            Test_INTER_060 ( ) && 
		      Test_INTER_070 ( ) && 
		      Test_INTER_080 ( ) &&
            Test_INTER_110 ( ) && 
		      Test_HMON_040 ( ) );
}  /* General_API_Test */

/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                  END OF FILE: AVTTestTypes.c                       | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
