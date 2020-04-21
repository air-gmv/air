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
 * | FILE: AuxMasterProcess.c                                           | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: AUXMASTERPROCESS                                        | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): AuxProcess                                  | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    17/10/2002                                                | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "AuxMasterProcess.h"

extern int              NrTestId;
extern int              TraceActive;

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CleanReceiveQueuingPort                             | *
 * |                                                                    | *
 * | DESCRIPTION: Puts all general purpose ARINC 653 objects declared   | *
 * |              on initialization state and ready to be used          | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   13/01/2005                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void CleanReceiveQueuingPort ( BOOLEAN IsFull )
{
   if ( IsFull == TRUE )
   {
	   CleanFullQueuingPort ( "QP_AUX_RECEIVE" );
   }
   else
   {
	   CleanQueuingPort ( "QP_AUX_RECEIVE" );
   }
   WriteResultsFromAuxPartition ( TRUE );
	
} /* CleanReceiveQueuingPort */ 

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: ExecuteAuxTest                                         | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   04/11/2004                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void ExecuteAuxTest ( APEX_INTEGER TestId )
{
	BOOLEAN TestResult = FALSE;
	
	printf("ExecuteAuxTest %d\n", TestId);

   TraceActive = AVT_NO_ACTIVE;
/*   PrepareTestEnvironmentOfAuxPartition ( );*/
	/* switch the test to execute*/
   switch( TestId )
   {
      case T_API_PART_230_0020 :
        #ifdef  TRACE_T_API_PART_230_0020                                 
           TraceActive = AVT_ACTIVE;
        #else	
           TraceActive = AVT_NO_ACTIVE;  
        #endif 
        AuxProcedure_T_API_PART_230_0020( &TestResult );
        break;
		  
      case T_API_PART_230_0030 :
        #ifdef  TRACE_T_API_PART_230_0030                                 
           TraceActive = AVT_ACTIVE;
        #else	
           TraceActive = AVT_NO_ACTIVE;  
        #endif 
        AuxProcedure_T_API_PART_230_0030( &TestResult );
        break;
		  
      case T_API_PART_230_0035 :                              
        #ifdef  TRACE_T_API_PART_230_0035                                 
           TraceActive = AVT_ACTIVE;
        #else	
           TraceActive = AVT_NO_ACTIVE;  
        #endif 
        AuxProcedure_T_API_PART_230_0035( &TestResult );
        break;

      default:
        break;
   }
} /* ExecuteAuxTest */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: AuxMasterProcess                                    | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   20/09/2002                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */

void AuxMasterProcess ()
{
   RETURN_CODE_TYPE     RetCode = NO_ERROR;
   APEX_BYTE            TestMessage[64];	
	APEX_INTEGER         TestId;
		
	printf("AuxMasterProcess\n");

   if( RetCode != NO_ERROR )
   {
        WriteLogErrorFunc( RetCode,
		                     "AuxMasterProcess",
	 	                     "SEND_QUEUING_MESSAGE" );
   }

   int i = 0;
   while (TRUE)
   {
      memset (TestMessage, '0', sizeof ( TestMessage ) );

      printf("%d\n", i);
      i++;

      RetCode = ReceiveQueuingMessage ( "QP_SUBTESTID_RECEIVE",
                                        INFINITE_TIME_VALUE,
                                        TestMessage );
		if ( RetCode == NO_ERROR ){
			printf("AuxMasterProcess received message. %s\n", TestMessage);

	      TestId =  atoi( TestMessage );
			NrTestId = TestId;
			if ( TestId == LAST_TEST )
			{
				printf("SET_PARTITION_MODE idle\n");
  			   SET_PARTITION_MODE ( IDLE, &RetCode );   
			}
			else
			{

			   ExecuteAuxTest ( TestId );
			}
		}else printf("ReceiveQueuingMessage failed %d\n", RetCode);
	}
}


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                    END OF FILE: AuxMasterProcess.c                 | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
