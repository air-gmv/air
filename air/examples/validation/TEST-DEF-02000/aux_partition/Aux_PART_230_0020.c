
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
 * | FILE: Aux_PART_230_0020.c                                          | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_PART                                           | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): AuxProcedure_T_API_PART_230_0020            | *
 * |                        SubAuxP1_T_API_PART_230_0020                | *
 * |                                                                    | *
 * | DESCRIPTION: TEST THE SERVICE SET_PARTITION_MODE TO IDLE.          | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    04/10/2002                                                | *
 * |                                                                    | *
 * | CVRS COVERED: API-PART-230:0020                                    | * 
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Aux_PART_230_0020.h"
#include "AVT_Funcs.h"
#include "bb.h"
#include "start.h"

extern int TraceActive;

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: AuxProcedure_T_API_PART_230_0020                    | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   04/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
AuxProcedure_T_API_PART_230_0020( BOOLEAN *TestResult )
{
  RETURN_CODE_TYPE         RetCode = NO_ERROR;
  BOOLEAN                  FlagOK  = TRUE; 
  APEX_BYTE                Message[QUEUING_PORT_MAX_STARTUPINFO_SIZE];
  char id[3];
  memset(id, 0, 3);
  int rc = -1;

  printf("AuxProcedure_T_API_PART_230_0020\n");

  /* initialize result */
  *TestResult = TRUE;
  
  /* Make the necessary procedures to Start SubAuxProcess_P1 */
  //sprintf( Message, "%d", P1_T_API_PART_230_0020 );

/*  test_id(P1_T_API_PART_230_0020, id);
  strcat((char *)Message, id);

  RetCode = DisplayBlackboard( "SubBBAuxId", Message, strlen(Message));
  EvaluateError( RetCode, &FlagOK );
  RetCode = StartStopProcess( "SubAuxProcess_P1", START_PROCESS );
*/


  rc = write_bb("1", 1);
  if(rc != 0) printf("write_bb failed\n");


  RetCode = start("SubAuxProcess_P1");
  if(RetCode != NO_ERROR){
	  printf("start failed\n");
	  *TestResult = FALSE;
	  return;
  }

  printf("started SubAuxProcess_P1\n");


  memset ( Message, '\0', QUEUING_PORT_MAX_STARTUPINFO_SIZE );
  strcpy ( Message, "RUN" );
	
  printf("aux_partition sending message: %s\n", Message);

  /* Send messages trought queuing port */
  RetCode = SendQueuingMessage ( "QP_AUXPARTITONSTARTUP_SEND",
                                 Message,
                                 QUEUING_PORT_MAX_STARTUPINFO_SIZE,
                                 INFINITE_TIME_VALUE,
                                 NO_ERROR );

  *TestResult = FALSE;

}  /* AuxProcedure_T_API_PART_230_0020 */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: SubAuxP1_T_API_PART_230_0020                        | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   04/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
SubAuxP1_T_API_PART_230_0020( void )
{
  RETURN_CODE_TYPE   RetCode = NO_ERROR;

  printf("SubAuxP1_T_API_PART_230_0020\n");
  /* call SetPartitionMode procedure */
  w_SET_PARTITION_MODE ( IDLE, &RetCode ); 

}  /* SubAuxP1_T_API_PART_230_0020  */


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Aux_PART_230_0020.c                    | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
