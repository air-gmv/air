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
 * | FILE: AuxPartitionMain.c                                           | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: AUXPARTITIONMAIN                                        | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): AuxPartitionMain                            | *
 * |                                                                    | *
 * | DESCRIPTION: Resources configuration from Auxiliary partition      | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    20/09/2002                                                | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/
#include "AuxPartitionMain.h"
#include "create_task.h"
#include "bb.h"

#define APX_PRIO_USR 15

avt_ctx_t *tasks_ctx;
extern int TraceActive;



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: SubAuxProcess                                       | *
 * |                                                                    | *
 * | DESCRIPTION: Resources configuration                               | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   20/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */




void create_ports(){

	RETURN_CODE_TYPE          RetCode = NO_ERROR;
	SAMPLING_PORT_ID_TYPE     SamplingPortId;
	QUEUING_PORT_ID_TYPE      QueuingPortId;
	  /**************************************************/
	  /**************************************************/
	  /*****                                        *****/
	  /*****            SAMPLING PORTS              *****/
	  /*****                                        *****/
	  /**************************************************/
	  /**************************************************/

	  /* Create sampling port "SP_AUX_RECEIVE" */
	  w_CREATE_SAMPLING_PORT( "SP_AUX_RECEIVE",
				  SAMPLING_PORT_MAX_MESSAGE_SIZE,
				  DESTINATION,
				  SAMPLING_PORT_REFRESH_TIME,
				  &SamplingPortId,
				  &RetCode);
	  /* error log */
	  if( RetCode != NO_ERROR )
	  {
	    WriteLogErrorFunc( RetCode,
			       "AuxPartitionMain",
			       "CREATE_SAMPLING_PORT SP_AUX_RECEIVE" );
	  }

	  /* Create sampling port "SP_AUX_SEND" */
	  w_CREATE_SAMPLING_PORT( "SP_AUX_SEND",
				  SAMPLING_PORT_MAX_MESSAGE_SIZE,
				  SOURCE,
				  SAMPLING_PORT_REFRESH_TIME,
				  &SamplingPortId,
				  &RetCode);
	  /* error log */
	  if( RetCode != NO_ERROR )
	  {
	    WriteLogErrorFunc( RetCode,
			       "AuxPartitionMain",
			       "CREATE_SAMPLING_PORT SP_AUX_SEND" );
	  }
	   /**************************************************/
	  /**************************************************/
	  /*****                                        *****/
	  /*****            QUEUING PORTS               *****/
	  /*****                                        *****/
	  /**************************************************/
	  /**************************************************/

	  /* Create Queuing port "QP_RESULT_SEND" */
	  w_CREATE_QUEUING_PORT( "QP_RESULT_SEND",
	  	       QUEUING_PORT_MAX_MESSAGE_SIZE,
				 QUEUING_PORT_MAX_MESSAGE_NB,
				 SOURCE,
				 FIFO,
				 &QueuingPortId,
				 &RetCode);
	  /* error log */
	  if( RetCode != NO_ERROR )
	  {
	    WriteLogErrorFunc( RetCode,
			       "AuxPartitionMain",
			       "CREATE_QUEUING_PORT QP_RESULT_SEND" );
	  }

	  /* Create Queuing port "QP_SUBTESTID_RECEIVE" */
	  w_CREATE_QUEUING_PORT( "QP_SUBTESTID_RECEIVE",
	  	       QUEUING_PORT_MAX_MESSAGE_SIZE,
				 QUEUING_PORT_MAX_MESSAGE_NB,
				 DESTINATION,
				 FIFO,
				 &QueuingPortId,
				 &RetCode);
	  /* error log */
	  if( RetCode != NO_ERROR )
	  {
	    WriteLogErrorFunc( RetCode,
			       "AuxPartitionMain",
			       "CREATE_QUEUING_PORT QP_SUBTESTID_RECEIVE" );
	  }

	  /* Create Queuing port "QP_AUXPARTITONSTARTUP_RECEIVE" */
	  w_CREATE_QUEUING_PORT( "QP_AUXPARTITONSTARTUP_SEND",
				 QUEUING_PORT_MAX_STARTUPINFO_SIZE,
				 QUEUING_PORT_MAX_MESSAGE_NB,
				 SOURCE,
				 FIFO,
				 &QueuingPortId,
				 &RetCode);
	  if( RetCode != NO_ERROR )
	  {
	    WriteLogErrorFunc( RetCode,
			       "AuxPartitionMain",
			       "CREATE_QUEUING_PORT QP_AUXPARTITONSTARTUP_SEND" );
	  }

	  /* Create Queuing port "QP_AUX_SEND" */
	  w_CREATE_QUEUING_PORT( "QP_AUX_SEND",
	  	       QUEUING_PORT_MAX_MESSAGE_SIZE,
				 QUEUING_PORT_MAX_MESSAGE_NB,
				 SOURCE,
				 FIFO,
				 &QueuingPortId,
				 &RetCode);
	  /* error log */
	  if( RetCode != NO_ERROR )
	  {
	    WriteLogErrorFunc( RetCode,
			       "AuxPartitionMain",
			       "CREATE_QUEUING_PORT QP_AUX_SEND" );
	  }

	  /* Create Queuing port "QP_AUX_RECEIVE" */
	  w_CREATE_QUEUING_PORT( "QP_AUX_RECEIVE",
	  	       QUEUING_PORT_MAX_MESSAGE_SIZE,
				 QUEUING_PORT_MAX_MESSAGE_NB,
				 DESTINATION,
				 FIFO,
				 &QueuingPortId,
				 &RetCode);
	  /* error log */
	  if( RetCode != NO_ERROR )
	  {
	    WriteLogErrorFunc( RetCode,
			       "AuxPartitionMain",
			       "CREATE_QUEUING_PORT QP_AUX_RECEIVE" );
	  }





}



void AuxPartitionMain( void ){

  RETURN_CODE_TYPE          RetCode = NO_ERROR;
  SAMPLING_PORT_ID_TYPE     SamplingPortId;
  QUEUING_PORT_ID_TYPE      QueuingPortId;
  PARTITION_STATUS_TYPE     StartUpPartitionStatus;  
  StartUpInfoType           StartUpInfo;
  APEX_BYTE                 StartUpMessage[QUEUING_PORT_MAX_STARTUPINFO_SIZE];

  TraceActive = AVT_NO_ACTIVE;
  
  /*load names of test on table*/
  LoadTestsName();
  
  /* create the processes */

  /**************************************************/
  /**************************************************/
  /*****                                        *****/
  /*****            SAMPLING PORTS              *****/
  /*****                                        *****/
  /**************************************************/
  /**************************************************/

  /* Create sampling port "SP_AUX_SEND" */
  w_CREATE_SAMPLING_PORT( "SP_AUX_SEND",
			  SAMPLING_PORT_MAX_MESSAGE_SIZE,
			  SOURCE,
			  SAMPLING_PORT_REFRESH_TIME,
			  &SamplingPortId,
			  &RetCode);
  /* error log */
  if( RetCode != NO_ERROR )
  {
    WriteLogErrorFunc( RetCode,
		       "AuxPartitionMain",
		       "CREATE_SAMPLING_PORT SP_AUX_SEND" );
  }

   /**************************************************/
  /**************************************************/
  /*****                                        *****/
  /*****            QUEUING PORTS               *****/
  /*****                                        *****/
  /**************************************************/
  /**************************************************/
 
  /* Create Queuing port "QP_RESULT_SEND" */
  w_CREATE_QUEUING_PORT( "QP_RESULT_SEND",
  	       QUEUING_PORT_MAX_MESSAGE_SIZE,
			 QUEUING_PORT_MAX_MESSAGE_NB,
			 SOURCE,
			 FIFO,
			 &QueuingPortId,
			 &RetCode);
  /* error log */
  if( RetCode != NO_ERROR )
  {
    WriteLogErrorFunc( RetCode,
		       "AuxPartitionMain",
		       "CREATE_QUEUING_PORT QP_RESULT_SEND" );
  }

  /* Create Queuing port "QP_SUBTESTID_RECEIVE" */
  w_CREATE_QUEUING_PORT( "QP_SUBTESTID_RECEIVE",
  	       QUEUING_PORT_MAX_MESSAGE_SIZE,
			 QUEUING_PORT_MAX_MESSAGE_NB,
			 DESTINATION,
			 FIFO,
			 &QueuingPortId,
			 &RetCode);
  /* error log */
  if( RetCode != NO_ERROR )
  {
    WriteLogErrorFunc( RetCode,
		       "AuxPartitionMain",
		       "CREATE_QUEUING_PORT QP_SUBTESTID_RECEIVE" );
  }

  /* Create Queuing port "QP_AUXPARTITONSTARTUP_RECEIVE" */
  w_CREATE_QUEUING_PORT( "QP_AUXPARTITONSTARTUP_SEND",
			 QUEUING_PORT_MAX_STARTUPINFO_SIZE,
			 QUEUING_PORT_MAX_MESSAGE_NB,
			 SOURCE,
			 FIFO,
			 &QueuingPortId,
			 &RetCode);
  if( RetCode != NO_ERROR )
  {
    WriteLogErrorFunc( RetCode,
		       "AuxPartitionMain",
		       "CREATE_QUEUING_PORT QP_AUXPARTITONSTARTUP_SEND" );
  }
  
  /* Create Queuing port "QP_AUX_SEND" */
  w_CREATE_QUEUING_PORT( "QP_AUX_SEND",
  	       QUEUING_PORT_MAX_MESSAGE_SIZE,
			 QUEUING_PORT_MAX_MESSAGE_NB,
			 SOURCE,
			 FIFO,
			 &QueuingPortId,
			 &RetCode);
  /* error log */
  if( RetCode != NO_ERROR )
  {
    WriteLogErrorFunc( RetCode,
		       "AuxPartitionMain",
		       "CREATE_QUEUING_PORT QP_AUX_SEND" );
  }


  
  /* Create Queuing port "QP_AUX_RECEIVE" */
  w_CREATE_QUEUING_PORT( "QP_AUX_RECEIVE",
  	       QUEUING_PORT_MAX_MESSAGE_SIZE,
			 QUEUING_PORT_MAX_MESSAGE_NB,
			 DESTINATION,
			 FIFO,
			 &QueuingPortId,
			 &RetCode);
  /* error log */
  if( RetCode != NO_ERROR )
  {
    WriteLogErrorFunc( RetCode,
		       "AuxPartitionMain",
		       "CREATE_QUEUING_PORT QP_AUX_RECEIVE" );
  }


  



  GET_PARTITION_STATUS (&StartUpPartitionStatus, &RetCode);
  StartUpInfo.OperatingMode = StartUpPartitionStatus.OPERATING_MODE;
  StartUpInfo.StartCondition = StartUpPartitionStatus.START_CONDITION;
    
   memset (StartUpMessage, '\0', sizeof ( StartUpMessage ) );
   memcpy (StartUpMessage, (char *) &StartUpInfo, sizeof ( StartUpInfoType ) );


   /* send message to Master Partition */
   RetCode = SendQueuingMessage ( "QP_AUXPARTITONSTARTUP_SEND", 
	 			                      StartUpMessage, 
				                      QUEUING_PORT_MAX_STARTUPINFO_SIZE,
				                      0, 
	 			                      NO_ERROR );
	
  /* Set partition to NORMAL state */
  SET_PARTITION_MODE( NORMAL, &RetCode );
  /* error log */
  if( RetCode != NO_ERROR )
  {
    WriteLogErrorFunc( RetCode,
		                 "AuxPartitionMain",
		                 "SET_PARTITION_MODE NORMAL" );
  }
  
} /* AuxPartitionMain  */




int Initial() {

	int rc = -1;
	avt_ctx_t ctx1;
	avt_ctx_t ctx2;

	tasks_ctx = NULL;

	AuxPartitionMain();

	rc = create_bb();
	if(rc != 0) printf("failed to create bb\n");

	ctx1.entry = (SYSTEM_ADDRESS_TYPE *) AuxMasterProcess;
	ctx1.id = -1;
	ctx1.prio = AUX_REGULAR_MASTER_PROCESS_PRIORITY;
	strcat(ctx1.name, "A");
	ctx1.nxt = NULL;

	rc = avt_create_rtems_task(&ctx1);
	if(rc != 0){
		printf("Failed to create AuxMasterProcess\n");
		return -1;
	}

	ctx2.entry = (SYSTEM_ADDRESS_TYPE *) SubAuxProcess;
	ctx2.id = -1;
	ctx2.prio = AUX_REGULAR_P1_PROCESS_PRIORITY;
	strcat(ctx2.name, "B");
	ctx2.nxt = NULL;

	rc = avt_create_rtems_task(&ctx2);
	if(rc != 0){
		printf("Failed to create SubAuxProcess\n");
		return -1;
	}

	ctx1.nxt = &ctx2;

	tasks_ctx = &ctx1;

	return 0;
}



/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                  END OF FILE: AuxPartitionMain.c                   | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
