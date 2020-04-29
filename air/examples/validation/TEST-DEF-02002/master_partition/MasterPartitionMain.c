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
 * | FILE: MasterPartitionMain.c                                        | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME:                                                         | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): StartPartition                              | *
 * |                                                                    | *
 * | DESCRIPTION: Create masterPartition resources, and performe some   | *
 * |              tests.                                                | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    19/09/2002                                                | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "MasterPartitionMain.h"
#include "create_task.h"

TEST_RESULT_TYPE TabResults[LAST_TEST];
extern int NrTestId;
extern int TraceActive;

extern QUEUING_PORT_ID_TYPE     QueuingPortIdReference;

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: ExecuteTestOnStartUp                                | *
 * |                                                                    | *
 * | DESCRIPTION: Execute the tests that must be performed when         / *
 * /              partition is starting                                 | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   20/01/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */

void ExecuteTestOnStartUp ( )
{

	printf("ExecuteTestOnStartUp\n");

  /* Test T-API-INTER-570:0011 */
  /* CREATE Queuing port "QP_CREATE_TEST" */
#ifdef EXECUTE_T_API_INTER_570_0011
  printf("EXECUTE_T_API_INTER_570_0011\n");
  #ifdef  TRACE_T_API_INTER_570_0011
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif

  NrTestId = T_API_INTER_570_0011;
  Test_T_API_INTER_570_0011 ( &(TabResults[T_API_INTER_570_0011]) );
#endif

  /* Test T-API-INTER-570:0012 */
  /* To test CheckCreateQueuingPort function, with  */
  /* invalid parameters and out of range */
#ifdef EXECUTE_T_API_INTER_570_0012

  #ifdef  TRACE_T_API_INTER_570_0012
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif

  NrTestId = T_API_INTER_570_0012;
  Test_T_API_INTER_570_0012 ( &(TabResults[T_API_INTER_570_0012]) );
#endif


TraceActive = AVT_NO_ACTIVE;
    
}

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: StartPartition                                      | *
 * |                                                                    | *
 * | DESCRIPTION: As described above.                                   | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void StartPartition ( void ){

  RETURN_CODE_TYPE            RetCode  = NO_ERROR;
  QUEUING_PORT_ID_TYPE        QueuingPortId = 0;
  SAMPLING_PORT_ID_TYPE       SamplingPortId = 0;
  int                         i = 0;
  char InitGlobalVariablesErrorMsg [INIT_ERRORMSG_SIZE];

  TraceActive = AVT_NO_ACTIVE;

  /* Initialize table of results */
  for( i = 0; i < LAST_TEST; i++ )
  {
    TabResults[i] = NOT_PERFORMED;
  }

  /*load names of test on table*/
  LoadTestsName ();
  
  /*****************************************************/
  /*****************************************************/
  /*****                                           *****/
  /*****             SAMPLING PORTS                *****/
  /*****                                           *****/
  /*****************************************************/
  /*****************************************************/

  /* Create Sampling port "SP_MASTER_RECEIVE" */
  w_CREATE_SAMPLING_PORT ( "SP_MASTER_RECEIVE",
				               SAMPLING_PORT_MAX_MESSAGE_SIZE,
				               DESTINATION,
				               SAMPLING_PORT_REFRESH_TIME,
			        	       &SamplingPortId,
			        	       &RetCode );  
  if( RetCode != NO_ERROR )
  {
    WriteLogErrorFunc( RetCode,
		       "MasterPartitionMain",
		       "CREATE_SAMPLING_PORT SP_MASTER_RECEIVE" );
  }

  /**************************************************/
  /**************************************************/
  /*****                                        *****/
  /*****            QUEUING PORTS               *****/
  /*****                                        *****/
  /**************************************************/
  /**************************************************/
  
  /* Create Queuing port "QP_AUXPARTITONSTARTUP_RECEIVE" */
  w_CREATE_QUEUING_PORT( "QP_AUXPARTITONSTARTUP_RECEIVE",
       QUEUING_PORT_MAX_STARTUPINFO_SIZE,
		   QUEUING_PORT_MAX_MESSAGE_NB,
			 DESTINATION,
			 FIFO,
			 &QueuingPortId,
			 &RetCode);
  if( RetCode != NO_ERROR )
  {
    WriteLogErrorFunc( RetCode,
		       "MasterPartitionMain",
		       "CREATE_QUEUING_PORT QP_AUXPARTITONSTARTUP_RECEIVE" );
  }
    
  /* Create Queuing port "QP_SUBTESTID_SEND" */
  w_CREATE_QUEUING_PORT( "QP_SUBTESTID_SEND",
			 QUEUING_PORT_MAX_MESSAGE_SIZE,
			 QUEUING_PORT_MAX_MESSAGE_NB,
			 SOURCE,
			 FIFO,
			 &QueuingPortId,
			 &RetCode);
  if( RetCode != NO_ERROR )
  {
    WriteLogErrorFunc( RetCode,
		       "MasterPartitionMain",
		       "CREATE_QUEUING_PORT QP_SUBTESTID_SEND" );
  }

  /* Create Queuing port "QP_RESULT_RECEIVE" */
  w_CREATE_QUEUING_PORT( "QP_RESULT_RECEIVE",
			 QUEUING_PORT_MAX_MESSAGE_SIZE,
			 QUEUING_PORT_MAX_MESSAGE_NB,
			 DESTINATION,
			 FIFO,
			 &QueuingPortId,
			 &RetCode);
  if( RetCode != NO_ERROR )
  {
    WriteLogErrorFunc( RetCode,
		       "MasterPartitionMain",
		       "CREATE_QUEUING_PORT QP_RESULT_RECEIVE" );
  }

  /* Create Queuing port "QP_MASTER_SEND" */
  w_CREATE_QUEUING_PORT( "QP_MASTER_SEND",
			 QUEUING_PORT_MAX_MESSAGE_SIZE,
			 QUEUING_PORT_MAX_MESSAGE_NB,
			 SOURCE,
			 FIFO,
			 &QueuingPortId,
			 &RetCode);
  if( RetCode != NO_ERROR )
  {
    WriteLogErrorFunc( RetCode,
		       "MasterPartitionMain",
		       "CREATE_QUEUING_PORT QP_MASTER_SEND" );
  }

  /*kept value for future check*/
  QueuingPortIdReference = QueuingPortId;

 
  /* Create Queuing port "QP_MASTER_RECEIVE" */
  w_CREATE_QUEUING_PORT( "QP_MASTER_RECEIVE",
			 QUEUING_PORT_MAX_MESSAGE_SIZE,
			 QUEUING_PORT_MAX_MESSAGE_NB,
			 DESTINATION,
			 FIFO,
			 &QueuingPortId,
			 &RetCode);
  if( RetCode != NO_ERROR )
  {
    WriteLogErrorFunc( RetCode,
		       "MasterPartitionMain",
		       "CREATE_QUEUING_PORT QP_SUBTESTID_SEND" );
  }
  
  
	/* Initialize Global Variables for this partition*/
	
	RetCode = InitGlobalVariables(InitGlobalVariablesErrorMsg);
	
	if (RetCode != NO_ERROR)
	{	
		printf("%s \n", InitGlobalVariablesErrorMsg);
	}

	/* execute tests that must be executed on normal mode */
  	ExecuteTestOnStartUp ();
	

  /* Test T-API-PART-230:0010 */
  /* Set partition mode to NORMAL */
#ifdef EXECUTE_T_API_PART_230_0010
  #ifdef  TRACE_T_API_PART_230_0010
     TraceActive = AVT_ACTIVE;
  #else	
     TraceActive = AVT_NO_ACTIVE;
  #endif

  NrTestId = T_API_PART_230_0010;
  TabResults[T_API_PART_230_0010] = FAILED;
  Test_T_API_PART_230_0010( &(TabResults[T_API_PART_230_0010]) );

#else	
  w_SET_PARTITION_MODE ( NORMAL, &RetCode );   
#endif


} /* StartPartition */

int Initial() {

	int rc = -1;
	avt_ctx_t ctx1;

	StartPartition();

	//MasterTestMain();

	ctx1.entry = (SYSTEM_ADDRESS_TYPE *) MasterTestMain;
	ctx1.id = -1;
	ctx1.prio = REGULAR_MASTER_PROCESS_PRIORITY;
	strcat(ctx1.name, "A");
	ctx1.nxt = NULL;

	rc = avt_create_rtems_task(&ctx1);
	if(rc != 0){
		printf("Failed to create AuxMasterProcess\n");
		return -1;
	}

	return 0;
}

/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                 END OF FILE: MasterPartitionMain                   | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
