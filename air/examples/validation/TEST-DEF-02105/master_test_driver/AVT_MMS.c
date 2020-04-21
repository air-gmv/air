/** 
 * @file AVT_MMS.c
 * 
 *  COPYRIGHT (c) 2013
 *  GMV-SKYSOFT 
 *
 * @author Bruno Oliveira
 * @brief  A653-P3 MMS Macro CheckSetModuleSchedule along auxiliary
 *		functions to store and manage queued messages
 * @note The functions contained in this file should be target and 
 *		implementation independent, i.e., compatible with any SO
 *		and testsuite
 * 
 */



#include "AVT_MMS.h"

BOOLEAN Test_Not_Over = TRUE;
APEX_INTEGER CycleCounter = 0;
char Message_Addr[STR_SIZE];
char Message_Stack[STACK_SIZE][STR_SIZE]; 



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: StoreMessage		                                | *
 * |                                                                    | *
 * | DESCRIPTION: Store received messages from Partition_1 and *_2      | *
 * |              in a message stack.                                   | *
 * |                                                                    | *
 * | AUTHOR: Bruno Oliveira                                             | *
 * | DATE:   20/09/2013                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | * 
 * |                                                                    | *
 * | RETURN VALUE:  BOOLEAN										        | *                | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */


BOOLEAN StoreMessage (int *StackIndex) {

	int i;

	if ( *StackIndex > STACK_SIZE ) {
		return FALSE;
	}
	if ( Message_Addr == NULL ) {
		return FALSE;
	}
	if ( Message_Stack == NULL) {
		return FALSE;
	}



	APEX_LONG_INTEGER *fMessage = (APEX_LONG_INTEGER *)&Message_Addr[0];
	APEX_LONG_INTEGER *fStack = 
		(APEX_LONG_INTEGER *)&(Message_Stack[*StackIndex][0]);
	
	for (i = 0; i < MSG_PARAMETERS; i++) {
		fStack[i] = fMessage[i];
	}

	#ifdef DEBUG
	printf("Recv from %ld\n", (long)fMessage[0]);
	#endif

	(*StackIndex)++;
	if (*StackIndex > STACK_SIZE) {
		*StackIndex = 0; /*Cycle*/
	}

	

	return TRUE;
	
}



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CheckMessageOrder                                   | *
 * |                                                                    | *
 * | DESCRIPTION: Check if the order of stored partition IDs (received  | *
 * |     via queuing ports) coincides with the correct order.           | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR: Bruno Oliveira                                             | *
 * | DATE:   19/09/2013                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | * 
 * |                                                                    | *
 * | RETURN VALUE:  ORDER_RESULT_TYPE										                | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */


BOOLEAN CheckMessageOrder( int count ) {

	int i;
	APEX_LONG_INTEGER *fStack = NULL;
	extern APEX_LONG_INTEGER Correct_Order[STACK_SIZE];

	#ifdef DEBUG
		for (i=0; i < count; i++) {
				fStack = (APEX_LONG_INTEGER *)&(Message_Stack[i][0]);
				printf("%ld ", (long)fStack[0]);
			}
			printf("\n");
	#endif


	if (count < 1) {
		return FALSE;
	}

	for (i = 0; i < count ; i++) {

		fStack = (APEX_LONG_INTEGER *)&(Message_Stack[i][0]);

		if (fStack == NULL) {
			return FALSE;
		}

		
		/*printf("i=%d, curr: %ld corr: %ld", 
			i,  
			(long)(fStack[0]), 
			(long)(Correct_Order[i])
			);*/

		if (fStack[0] != Correct_Order[i]) {
			printf("!Wrong Message/Partition Order!\n");
			printf("\n");
			return FALSE;
		}

	}

	return TRUE;

}







BOOLEAN isPart3InSched( void ) {

	return FALSE;

}

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CheckSetModuleSchedule                              | *
 * |                                                                    | *
 * | DESCRIPTION: Implementation of the ARINC P3 MMS 										|*
 * |        CheckSetModuleSchedule Macro				            						| *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR: Bruno Oliveira                                             | *
 * | DATE:   19/09/2013                                                 | *
 * |                                                                    | *
 * |                                                                    | * 
 * |                                                                    | *
 * | RETURN VALUE:  Matching_Error_Bits Structure                       | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */

void CheckSetModuleSchedule ( 
	SCHEDULE_ID_TYPE SCHEDULE_ID,
	Matching_Error_Bits *ReturnStruct, 
	RETURN_CODE_TYPE *RETURN_CODE
	) {

	RETURN_CODE_TYPE RetCode = NO_ERROR;
	BOOLEAN SetModuleScheduleControl = FALSE;
	BOOLEAN ErrorOccurred = FALSE;
	QUEUING_PORT_NAME_TYPE Queuing_Port_Name = "Test_1_Dq";
	QUEUING_PORT_ID_TYPE   Queuing_Port_Id = 0;
	MESSAGE_SIZE_TYPE Length = 0;
	int StackIndex = 0;
	APEX_LONG_INTEGER Time_Out_Set = 0;
	APEX_LONG_INTEGER Time_Out = 0;
	SCHEDULE_STATUS_TYPE SchedStatus;
	extern int Number_of_Cycles;

	#ifdef DEBUG
		printf("CheckSetModuleSchedule\n");
	#endif
	
	SetErrorBits(ReturnStruct);
	
	while ( Test_Not_Over ) {
		if (!SetModuleScheduleControl) {
			w_SET_MODULE_SCHEDULE (SCHEDULE_ID, &RetCode);
			SetModuleScheduleControl = TRUE;
			if (RetCode != NO_ERROR) {
				*RETURN_CODE = RetCode;
				/*set error bits here*/
			}
		}
		strncpy(Queuing_Port_Name, "Test_1_Dq", 10);
		w_GET_QUEUING_PORT_ID (
			Queuing_Port_Name, 
			&Queuing_Port_Id, 
			&RetCode); 
		if (RetCode != NO_ERROR) ErrorOccurred = TRUE;


		w_RECEIVE_QUEUING_MESSAGE (
			Queuing_Port_Id,
			25000000ll,
			(MESSAGE_ADDR_TYPE) Message_Addr,
			&Length,
			&RetCode );

		if (RetCode == NO_ERROR) {
			StoreMessage(&StackIndex);

			w_TIMED_WAIT(25000000ll, &RetCode);
			Time_Out_Set = 50000000ll;
		} else {
			Time_Out_Set = 25000000ll;
		}

		strncpy(Queuing_Port_Name, "Test_2_Dq", 10);
		w_GET_QUEUING_PORT_ID (
			Queuing_Port_Name, 
			&Queuing_Port_Id, 
			&RetCode); 
		if (RetCode != NO_ERROR) ErrorOccurred = TRUE;

		Time_Out = Time_Out_Set;
		w_RECEIVE_QUEUING_MESSAGE (
			Queuing_Port_Id,
			Time_Out,
			(MESSAGE_ADDR_TYPE) Message_Addr,
			&Length,
			&RetCode );


		if (RetCode == NO_ERROR) {

			StoreMessage(&StackIndex);
			w_TIMED_WAIT(25000000ll, &RetCode);
			if (RetCode != NO_ERROR) ErrorOccurred = TRUE;
			Time_Out_Set = 50000000ll;
		} else {
			Time_Out_Set = 25000000ll;
		}

		if ( isPart3InSched() ) {
			strncpy(Queuing_Port_Name, "Test_3_Dq\0", 10);
			w_GET_QUEUING_PORT_ID (
				Queuing_Port_Name, 
				&Queuing_Port_Id, 
				&RetCode); 
			if (RetCode != NO_ERROR) ErrorOccurred = TRUE;

			Time_Out = Time_Out_Set;
			w_RECEIVE_QUEUING_MESSAGE (
				Queuing_Port_Id,
				Time_Out,
				(MESSAGE_ADDR_TYPE) Message_Addr,
				&Length,
				&RetCode );
			if (RetCode == NO_ERROR) {
				StoreMessage(&StackIndex);
				w_TIMED_WAIT(25000000ll, &RetCode);
				if (RetCode != NO_ERROR) ErrorOccurred = TRUE;
				Time_Out_Set = 50000000ll;
			} else {
				Time_Out_Set = 25000000ll;
			}


		}

		/*ToDo: This must be related to the port results */
		if (!ErrorOccurred) {
			/* if messages were received and are in the correct order */
			CycleCounter++;

			#ifdef DEBUG
				printf("%d Cycles\n", CycleCounter);
			#endif

			if (CycleCounter > Number_of_Cycles) {
				Test_Not_Over = FALSE;
				printf("!Max Number of Cycles Reached!\n");
			}
		}

		if ( FALSE == CheckMessageOrder(StackIndex)) {
			ErrorOccurred = TRUE;
		}

		if (Test_Not_Over) {
			if (ErrorOccurred) {
				Test_Not_Over = FALSE;
				SetErrorBits(ReturnStruct);
				printf("!Error in CheckSetModuleSchedule!\n");
			}
		

			else if ( Test_Not_Over && CheckMessageOrder(StackIndex)
				&& (CycleCounter == Number_of_Cycles)
					) {
				Test_Not_Over = FALSE;
				SetNoErrorBits(
					SCHEDULE_ID,
					ReturnStruct, 
					StackIndex);
			}
		}



	}
	 
}

