#include "AVT_independent.h"
#include "AVT_dependent.h"
#include "Test_MMS_0040_0031.h"

char TestName[] = "Test_MMS_0040_0031";
QUEUING_PORT_NAME_TYPE Queuing_Port_Name = "Test_1_Sq\0";

RETURN_CODE_TYPE BuildMessage(
	char *Message, 
	PARTITION_STATUS_TYPE *PartitionStatus,
	SYSTEM_TIME_TYPE *System_Time) {

	if (Message == NULL)
		return NO_ACTION;

	APEX_LONG_INTEGER *fMessage = (APEX_LONG_INTEGER *)Message;

	fMessage[0] = (APEX_LONG_INTEGER)PartitionStatus->IDENTIFIER;
	fMessage[1] = (APEX_LONG_INTEGER)PartitionStatus->OPERATING_MODE;
	fMessage[2] = (APEX_LONG_INTEGER)PartitionStatus->PERIOD;
	fMessage[3] = (APEX_LONG_INTEGER)PartitionStatus->DURATION;
	fMessage[4] = (APEX_LONG_INTEGER)PartitionStatus->START_CONDITION;

	fMessage[5] = (APEX_LONG_INTEGER)*System_Time;


	/*printf("Create Msg: %ld %ld %ld %ld %ld", 
		fMessage[0], fMessage[1], fMessage[2], fMessage[3],
		fMessage[4], fMessage[5]); */

	return NO_ERROR;


}



void CheckSetModuleSchedulePartition (PARTITION_ID_TYPE PartitionId) {

	PARTITION_STATUS_TYPE Partition_Status;
	RETURN_CODE_TYPE ReturnCode;
	SYSTEM_TIME_TYPE System_Time;
	QUEUING_PORT_ID_TYPE   Queuing_Port_Id = 0;
	char Message_Addr[STR_SIZE];

	

	while (TRUE) {

		w_GET_PARTITION_STATUS(&Partition_Status, &ReturnCode);
		w_GET_TIME(&System_Time, &ReturnCode);


		w_GET_QUEUING_PORT_ID(
			Queuing_Port_Name, 
			&Queuing_Port_Id,
			&ReturnCode);

		BuildMessage(Message_Addr, &Partition_Status, &System_Time);

		w_SEND_QUEUING_MESSAGE (
			Queuing_Port_Id, 
			(MESSAGE_ADDR_TYPE) Message_Addr,
			49, /* 8 bytes * 5 msg +1 \0 */
			50000000ll,
			&ReturnCode );

		w_TIMED_WAIT(50000000ll, &ReturnCode);


	}

}



PARTITION_ID_TYPE PrintPartitionStatus( void ) {

	PARTITION_STATUS_TYPE PartStatus;
  PARTITION_ID_TYPE PartId;
  OPERATING_MODE_TYPE OperMode;
  RETURN_CODE_TYPE RetCode;

	w_GET_PARTITION_STATUS(&PartStatus, &RetCode);
	if (RetCode != NO_ERROR) {
		return (PARTITION_ID_TYPE)0;
	}

  PartId = PartStatus.IDENTIFIER;
  OperMode = PartStatus.OPERATING_MODE;


  printf("Partition %d, mode %d\n", 
  	(int)PartId,
  	(int)OperMode );

  return PartId;

}



/*ToDo: this can be partition independent */
void
Initial () {

	RETURN_CODE_TYPE RetCode = NO_ERROR;
  QUEUING_PORT_ID_TYPE PortId;
  PARTITION_ID_TYPE PartId;

  PartId = PrintPartitionStatus();
  if (PartId == (PARTITION_ID_TYPE)0) {
  	printf("PartitionStatus Error\n");
  	w_STOP_SELF();
  }

  switch (PartId) {
		case (PARTITION_ID_TYPE)1:
			strncpy(Queuing_Port_Name, "Test_1_Sq", 10);
			break;
		case (PARTITION_ID_TYPE)2:
			strncpy(Queuing_Port_Name, "Test_2_Sq", 10);
			break;
		case (PARTITION_ID_TYPE)3:
			strncpy(Queuing_Port_Name, "Test_3_Sq", 10);
			break;
		default:
			printf("CheckSetModuleSchedulePartition PartitionId invalid\n");
			return; /* ToDo: Return Codes */
			break;
	}

	w_CREATE_QUEUING_PORT(
  	Queuing_Port_Name, 
  	STR_SIZE,
  	4,
  	SOURCE,
  	FIFO,
  	&PortId,
  	&RetCode);

  w_SET_PARTITION_MODE ( NORMAL, &RetCode );

  while (1) {
  	CheckSetModuleSchedulePartition(PartId);
  }

}
