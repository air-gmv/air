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
 * | FILE: AVT_QueuingPorts.c                                           | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: QUEUINGPORTS                                            | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S):  CheckCreateQueuingPort                     | *
 * |                         CheckSendQueuingMessage                    | *
 * |                         CheckReceiveQueuingMessage                 | *
 * |                         CheckGetQueuingPortId                      | *
 * |                         CheckQueuingPortId                         | *
 * |                         CheckQueuingPortStatus                     | *
 * |                                                                    | *
 * | DESCRIPTION:   The goal of these functions, is to test the similar | *
 * |                functions of ARINC653,  passing them the input      | *
 * |                parameters of the ARINC653 functions and the        | *
 * |                expected values for the output parameters and       | *
 * |                verified that the obtained  values are the expected | *
 * |                ones.                                               | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    05/09/2002                                                | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "AVT_QueuingPorts.h"



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CheckCreateQueuingPort                              | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   05/09/2002                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | QUEUING_PORT_NAME_TYPE                                             | *
 * |                  InPortName     X            Name given to the     | *
 * |                                              queuing port (QP).    | *
 * | MESSAGE_SIZE_TYPE_TYPE                                             | *
 * |             InMaxMessageSize    X            Maximun message size. | *
 * | MESSAGE_RANGE_TYPE                                                 | *
 * |               InMaxNBMessage    X            Maximun number of     | *
 * |                                              messages allowed.     | *
 * | PORT_DIRECTION_TYPE                                                | *
 * |              InPortDirection    X            Direction(src/dest)   | *
 * |                                              given to the QP.      | *
 * |  QUEUING_DISCIPLINE_TYPE                                           | *
 * |             InPortDiscipline    X            Queuing discipline    | *
 * |                                              given to the QP.      | *
 * | RETURN_CODE_TYPE InReturnCode   X            Expected return code, | *
 * |                                              resulting from the    | *
 * |                                              call of the function  | *
 * |                                              under test.           | *
 * | QUEUING_PORT_ID_TYPE *OutPortId         X    Returned QP id        | *
 * |                                              resulting from the    | *
 * |                                              call of the function  | *
 * |                                              under test.           | *
 * | QUEUING_PORT_STATUS_TYPE *OutStatus     X    Structure filled with | *
 * |                                              information regarding | *
 * |                                              the status of the     | *
 * |                                              operation.            | *
 * | RETUEN_CODE_TYPE *OutReturnCode         X    Return code of the    | *
 * |                                              ARINC function that   | *
 * |                                              was called.           | *
 * | RETURN VALUE:                                                      | *
 * | CREATE_QUEUING_PORT_ERROR                    Bit structure holding | *
 * |                                              the different types of| *
 * |                                              errors that can occur | *
 * |                                              where the error that  | *
 * |                                              occurred has its bit  | *
 * |                                              set to one. The others! *
 * |                                              are set to 0.         | *
 * ---------------------------------------------------------------------- */
CREATE_QUEUING_PORT_ERROR
CheckCreateQueuingPort( QUEUING_PORT_NAME_TYPE    InPortName,
                        MESSAGE_SIZE_TYPE         InMaxMessageSize,
                        MESSAGE_RANGE_TYPE        InMaxNBMessage,
                        PORT_DIRECTION_TYPE       InPortDirection,
                        QUEUING_DISCIPLINE_TYPE   InPortDiscipline,
                        RETURN_CODE_TYPE          InReturnCode,
                        QUEUING_PORT_ID_TYPE     *OutPortId,
                        QUEUING_PORT_STATUS_TYPE *OutStatus,
                        RETURN_CODE_TYPE         *OutReturnCode )
{
  char *TabPortDir[2] = { "SOURCE", "DESTINATION" };
  CREATE_QUEUING_PORT_ERROR ReturnError;
  RETURN_CODE_TYPE          ReturnCode = NO_ERROR;

  printf("CheckCreateQueuingPort\n");

  /* Clean error struct */
  ReturnError.ReturnCode     = NO_ERROR;
  ReturnError.GetStatus      = NO_ERROR;
  ReturnError.MaxMessageSize = NO_ERROR;
  ReturnError.MaxNBMessage   = NO_ERROR;
  ReturnError.PortDirection  = NO_ERROR;

  /* Clean all output parameters */
  memset( (void*)OutStatus, '\0', sizeof(QUEUING_PORT_STATUS_TYPE));
  *OutReturnCode = NO_ERROR;
  *OutPortId     = 0;

  /* Call AIRBUS API function */
  w_CREATE_QUEUING_PORT(  InPortName,
        InMaxMessageSize,
        InMaxNBMessage,
        InPortDirection,
        InPortDiscipline,
        OutPortId,
        OutReturnCode );

  printf("OutReturnCode: %d\n", *OutReturnCode);

  /* Evaluate the results */

  /* RETURN CODE */
  if( *OutReturnCode != InReturnCode )
  {
    ReturnError.ReturnCode = AVT_ERROR;
    WriteLogReturnCode("CheckCreateQueuingPort", InReturnCode, *OutReturnCode);
  }

  /* If CREATE_QUEUING_PORT returns NO_ERROR, execute */
  /* GET_QUEUING_PORT__STATUS  to see if the attributes values */
  /* was well inserted. */
  if( *OutReturnCode == NO_ERROR )
  {
    memset((void*)OutStatus, '\0', sizeof(QUEUING_PORT_STATUS_TYPE));
    w_GET_QUEUING_PORT_STATUS( *OutPortId, OutStatus, &ReturnCode );
    if( ReturnCode == NO_ERROR )
    {
      /* MAX MESSAGE SIZE */
      if( OutStatus->MAX_MESSAGE_SIZE != InMaxMessageSize )
      {
        ReturnError.MaxMessageSize = AVT_ERROR;
        WriteLogErrorNum( "CheckCreateQueuingPort",
                          "MAX_MESSAGE_SIZE",
                          InMaxMessageSize,
                          OutStatus->MAX_MESSAGE_SIZE );
      }

      /* MAX NB MESSAGE */
      if( OutStatus->MAX_NB_MESSAGE != InMaxNBMessage )
      {
        ReturnError.MaxNBMessage = AVT_ERROR;
        WriteLogErrorNum( "CheckCreateQueuingPort",
                          "MAX_NB_MESSAGE",
                          InMaxNBMessage,
                          OutStatus->MAX_NB_MESSAGE );
      }

      /* PORT DIRECTION */
      if( OutStatus->PORT_DIRECTION != InPortDirection )
      {
         ReturnError.PortDirection = AVT_ERROR;
         WriteLogErrorStr( "CheckCreateQueuingPort",
                           "PORT_DIRECTION",
                           TabPortDir[InPortDirection],
                           TabPortDir[OutStatus->PORT_DIRECTION] );
      }
    }
    else
    {
      ReturnError.GetStatus = AVT_ERROR;
      WriteLogErrorFunc( ReturnCode,
                         "CheckCreateQueuingPort",
                         "GET_QUEUING_PORT_STATUS" );
    }
  }
  return ReturnError;

} /* CheckCreateQueuingPort */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CheckSendQueuingMessage                             | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   05/09/2002                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | QUEUING_PORT_ID_TYPE InPortId   X            QP identification.    | *
 * | MESSAGE_AREA_TYPE  *InMessage   X            Message to be sent    | *
 * |                                              through the QP.       | *
 * | MESSAGE_SIZE_TYPE InMessageSize X            Size of the message   | *
 * |                                              to be sen through QP. | *
 * | SYSTEM_TIME_TYPE InTimeOut      X            QP timeout            | *
 * | RETURN_CODE_TYPE InReturnCode   X            Expected return code, | *
 * |                                              resulting from the    | *
 * |                                              call of the function  | *
 * |                                              under test.           | *
 * | QUEUING_PORT_STATUS_TYPE *OutStatus     X    Structure filled with | *
 * |                                              information regarding | *
 * |                                              the status of the     | *
 * |                                              operation.            | *
 * | RETUEN_CODE_TYPE *OutReturnCode         X    Return code of the    | *
 * |                                              ARINC function that   | *
 * |                                              was called.           | *
 * | RETURN VALUE:                                                      | *
 * | SEND_QUEUING_MESSAGE_ERROR                   Bit structure holding | *
 * |                                              the different types of| *
 * |                                              errors that can occur | *
 * |                                              where the error that  | *
 * |                                              occurred has its bit  | *
 * |                                              set to one. The others! *
 * |                                              are set to 0.         | *
 * ---------------------------------------------------------------------- */
SEND_QUEUING_MESSAGE_ERROR
CheckSendQueuingMessage( QUEUING_PORT_ID_TYPE      InPortId,
       APEX_BYTE                *InMessage,
       MESSAGE_SIZE_TYPE         InMessageSize,
       SYSTEM_TIME_TYPE          InTimeOut,
       RETURN_CODE_TYPE          InReturnCode,
       QUEUING_PORT_STATUS_TYPE *OutStatus,
       RETURN_CODE_TYPE         *OutReturnCode )
{
  SEND_QUEUING_MESSAGE_ERROR ReturnError;
  RETURN_CODE_TYPE           ReturnCode = NO_ERROR;

  /* Clean error struct */
  ReturnError.ReturnCode    = NO_ERROR;
  ReturnError.GetStatus     = NO_ERROR;

  /* Clean all output parameters */
  memset( (void*)OutStatus, '\0', sizeof(QUEUING_PORT_STATUS_TYPE));
  *OutReturnCode = NO_ERROR;

  /* Call ARINC653 function */
  w_SEND_QUEUING_MESSAGE( InPortId,
        InMessage,
        InMessageSize,
        InTimeOut,
        OutReturnCode );

  /* Evaluate the results */

  /* RETURN CODE */
  if( *OutReturnCode != InReturnCode )
  {
    ReturnError.ReturnCode = AVT_ERROR;
    WriteLogReturnCode("CheckSendQueuingMessage",
           InReturnCode,
           *OutReturnCode );
  }

  /* If SEND_QUEUING_MESSAGE returns NO_ERROR, GET_QUEUING_PORT_STATUS */
   /* and return the status  (merely informative) */
  if( *OutReturnCode == NO_ERROR )
  {
    memset( (void*)OutStatus, '\0', sizeof(QUEUING_PORT_STATUS_TYPE));
    w_GET_QUEUING_PORT_STATUS( InPortId, OutStatus, &ReturnCode );
    if( ReturnCode != NO_ERROR )
    {
      ReturnError.GetStatus = AVT_ERROR;
      WriteLogErrorFunc( ReturnCode,
       "CheckSendQueuingMessage",
       "GET_QUEUING_PORT_STATUS" );
    }
  }
  return ReturnError;

} /* CheckSendQueuingMessage */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CheckReceiveQueuingMessage                          | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   05/09/2002                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | QUEUING_PORT_ID_TYPE InPortId   X            QP identification.    | *
 * | SYSTEM_TIME_TYPE    InTimeOut   X            QP timeout            | *
 * | MESSAGE_ADDR_TYPE                                                  | *
 * |                 InMessageAddr   X            Addr.of the expected  | *
 * |                                              received message      | *
 * | MESSAGE_SIZE_TYPE                                                  | *
 * |              InMessageLength    X            Expected message size | *
 * | RETURN_CODE_TYPE InReturnCode   X            Expected return code, | *
 * |                                              resulting from the    | *
 * |                                              call of the function  | *
 * |                                              under test.           | *
 * | MESSAGE_ADDR_TYPE  OutMessage           X    Addr. of the received | *
 * |                                              message.              | *
 * |                                              through the QP.       | *
 * | MESSAGE_SIZE_TYPE *OutMessageLength     X    Size of the received  | *
 * |                                              message.              | *
 * | QUEUING_PORT_STATUS_TYPE *OutStatus     X    Structure filled with | *
 * |                                              information regarding | *
 * |                                              the status of the     | *
 * |                                              operation.            | *
 * | RETUEN_CODE_TYPE *OutReturnCode         X    Return code of the    | *
 * |                                              ARINC function that   | *
 * |                                              was called.           | *
 * | RETURN VALUE:                                                      | *
 * | RECEIVE_QUEUING_MESSAGE_ERROR                Bit structure holding | *
 * |                                              the different types of| *
 * |                                              errors that can occur | *
 * |                                              where the error that  | *
 * |                                              occurred has its bit  | *
 * |                                              set to one. The others! *
 * |                                              are set to 0.         | *
 * ---------------------------------------------------------------------- */
RECEIVE_QUEUING_MESSAGE_ERROR
CheckReceiveQueuingMessage( QUEUING_PORT_ID_TYPE      InPortId,
          SYSTEM_TIME_TYPE          InTimeOut,
          APEX_BYTE                *InMessage,
          MESSAGE_SIZE_TYPE         InMessageLength,
          RETURN_CODE_TYPE          InReturnCode,
          APEX_BYTE                *OutMessage,
          MESSAGE_SIZE_TYPE        *OutMessageLength,
          QUEUING_PORT_STATUS_TYPE *OutStatus,
          RETURN_CODE_TYPE         *OutReturnCode )
{
  RECEIVE_QUEUING_MESSAGE_ERROR ReturnError;
  RETURN_CODE_TYPE              ReturnCode = NO_ERROR;

  /* Clean error struct */
  ReturnError.ReturnCode    = NO_ERROR;
  ReturnError.MessageSize   = NO_ERROR;
  ReturnError.Message       = NO_ERROR;
  ReturnError.GetStatus     = NO_ERROR;

  /* Clean all output parameters */
  memset( (void*)OutStatus, '\0', sizeof(QUEUING_PORT_STATUS_TYPE));
  *OutReturnCode    = NO_ERROR;
  *OutMessageLength = 0;


  /* Call ARINC653 function */
  w_RECEIVE_QUEUING_MESSAGE(  InPortId,
            InTimeOut,
            OutMessage,
            OutMessageLength,
            OutReturnCode );

  /* Evaluate the results */

  /* RETURN CODE */
  if( *OutReturnCode != InReturnCode )
  {
    ReturnError.ReturnCode = AVT_ERROR;
    WriteLogReturnCode("CheckReceiveQueuingMessage",
           InReturnCode,
           *OutReturnCode );
  }

  /* If RECEIVE_QUEUING_MESSAGE returns NO_ERROR,  */
  if( *OutReturnCode == NO_ERROR )
  {
    /* MESSASE LENGTH */
    if( *OutMessageLength != InMessageLength )
    {
      ReturnError.MessageSize = AVT_ERROR;
      WriteLogErrorNum( "CheckReceiveQueuingMessage",
      "MESSASE_LENGTH",
      InMessageLength,
      *OutMessageLength );
    }

    /* MESSAGE */
    else if( strncmp(  OutMessage, InMessage, *OutMessageLength ) != 0)
    {
      ReturnError.Message = AVT_ERROR;
      WriteLogErrorStr( "CheckReceiveQueuingMessage",
      "MESSAGE",
      InMessage,
      OutMessage );
    }

    /* GET_QUEUING_PORT_STATUS and return the status  (merely informative) */
    memset( (void*)OutStatus, '\0', sizeof(QUEUING_PORT_STATUS_TYPE));
    w_GET_QUEUING_PORT_STATUS( InPortId, OutStatus, &ReturnCode );
    if( ReturnCode != NO_ERROR )
    {
      ReturnError.GetStatus = AVT_ERROR;
      WriteLogErrorFunc( ReturnCode,
       "CheckReceiveQueuingMessage",
       "GET_QUEUING_PORT_STATUS" );
    }
  }
  return ReturnError;

} /* CheckReceiveQueuingMessage */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CheckGetQueuingPortId                               | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   05/09/2002                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | QUEUING_PORT_NAME_TYPE                                             | *
 * |              *InPortName        X            Name given to QP.     | *
 * | RETURN_CODE_TYPE InReturnCode   X            Expected return code, | *
 * |                                              resulting from the    | *
 * |                                              call of the function  | *
 * |                                              under test.           | *
 * |  QUEUING_PORT_ID_TYPE *OutPortId        X    Identification given  | *
 * |                                              to QP.                | *
 * | QUEUING_PORT_STATUS_TYPE *OutStatus     X    Structure filled with | *
 * |                                              information regarding | *
 * |                                              the status of the     | *
 * |                                              operation.            | *
 * | RETUEN_CODE_TYPE *OutReturnCode         X    Return code of the    | *
 * |                                              ARINC function that   | *
 * |                                              was called.           | *
 * | RETURN VALUE:                                                      | *
 * | GET_QUEUING_PORT_ID_ERROR                    Bit structure holding | *
 * |                                              the different types of| *
 * |                                              errors that can occur | *
 * |                                              where the error that  | *
 * |                                              occurred has its bit  | *
 * |                                              set to one. The others! *
 * |                                              are set to 0.         | *
 * ---------------------------------------------------------------------- */
GET_QUEUING_PORT_ID_ERROR
CheckGetQueuingPortId( QUEUING_PORT_NAME_TYPE    InPortName,
           RETURN_CODE_TYPE          InReturnCode,
           QUEUING_PORT_ID_TYPE     *OutPortId,
           QUEUING_PORT_STATUS_TYPE *OutStatus,
           RETURN_CODE_TYPE         *OutReturnCode )
{
  GET_QUEUING_PORT_ID_ERROR ReturnError;
  RETURN_CODE_TYPE          ReturnCode = NO_ERROR;

  /* Clean error struct */
  ReturnError.ReturnCode = NO_ERROR;
  ReturnError.GetStatus  = NO_ERROR;

  /* Clean all output parameters */
  memset( (void*)OutStatus, '\0', sizeof(QUEUING_PORT_STATUS_TYPE));
  *OutReturnCode = NO_ERROR;
  *OutPortId     = 0;

  /* Call ARINC653 function */
  w_GET_QUEUING_PORT_ID( InPortName, OutPortId, OutReturnCode );

  /* Evaluate the results */

  /* RETURN CODE */
  if( *OutReturnCode != InReturnCode )
  {
    ReturnError.ReturnCode = AVT_ERROR;
    WriteLogReturnCode("CheckGetQueuingPortId", InReturnCode, *OutReturnCode );
  }

  /* If NO_ERROR is returned */
  if( *OutReturnCode == NO_ERROR )
  {
    /* Execute, GET_QUEUING_PORT_STATUS */
    /* and return the status  (merely informative) */
    w_GET_QUEUING_PORT_STATUS( *OutPortId, OutStatus, &ReturnCode );
    if( ReturnCode != NO_ERROR )
    {
      ReturnError.GetStatus = AVT_ERROR;
      WriteLogErrorFunc( ReturnCode,
       "CheckGetQueuingPortId",
       "GET_QUEUING_PORT_STATUS" );
    }
  }
  return ReturnError;

} /* CheckGetQueuingPortId */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CheckQueuingPortStatus                              | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   05/09/2002                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | QUEUING_PORT_ID_TYPE InPortId   X            QP identification.    | *
 * | RETURN_CODE_TYPE InReturnCode   X            Expected return code, | *
 * |                                              resulting from the    | *
 * |                                              call of the function  | *
 * |                                              under test.           | *
 * | QUEUING_PORT_STATUS_TYPE *OutStatus     X    Structure filled with | *
 * |                                              information regarding | *
 * |                                              the status of the     | *
 * |                                              operation.            | *
 * | RETUEN_CODE_TYPE *OutReturnCode         X    Return code of the    | *
 * |                                              ARINC function that   | *
 * |                                              was called.           | *
 * | RETURN VALUE:                                                      | *
 * | GET_QUEUING_PORT_STATUS_ERROR                Bit structure holding | *
 * |                                              the different types of| *
 * |                                              errors that can occur | *
 * |                                              where the error that  | *
 * |                                              occurred has its bit  | *
 * |                                              set to one. The others! *
 * |                                              are set to 0.         | *
 * ---------------------------------------------------------------------- */
GET_QUEUING_PORT_STATUS_ERROR
CheckQueuingPortStatus( QUEUING_PORT_ID_TYPE      InPortId,
      RETURN_CODE_TYPE          InReturnCode,
      QUEUING_PORT_STATUS_TYPE *OutStatus,
      RETURN_CODE_TYPE         *OutReturnCode )
{
  GET_QUEUING_PORT_STATUS_ERROR ReturnError;

  /* Clean error struct */
  ReturnError.ReturnCode    = NO_ERROR;

  /* Clean all output parameters */
  memset( (void*)OutStatus, '\0', sizeof(QUEUING_PORT_STATUS_TYPE));
  *OutReturnCode = NO_ERROR;

  /* Call ARINC653 function */
  w_GET_QUEUING_PORT_STATUS( InPortId, OutStatus, OutReturnCode );

  /* RETURN CODE */
  if( *OutReturnCode != InReturnCode )
  {
    ReturnError.ReturnCode = AVT_ERROR;
    WriteLogReturnCode("CheckQueuingPortStatus", InReturnCode, *OutReturnCode);
  }
  return ReturnError;

} /* CheckQueuingPortStatus */


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                    END OF FILE:                                    | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
