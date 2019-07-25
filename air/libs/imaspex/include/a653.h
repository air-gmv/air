/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/
/**
 * @file a653.h
 * @author cdcs
 * @date 2011
 * @brief Includes all ARINC 653 related declarations and definitions
 */

#ifndef apx_a653_decl
#define apx_a653_decl

#include <air.h>

#define MAX_NUMBER_OF_PARTITIONS SYSTEM_LIMIT_NUMBER_OF_PARTITIONS

#define SYSTEM_LIMIT_NUMBER_OF_PARTITIONS 32         /* module scope       */
#define SYSTEM_LIMIT_NUMBER_OF_MESSAGES 512          /* module scope       */
#define SYSTEM_LIMIT_MESSAGE_SIZE 8192               /* module scope       */
#define SYSTEM_LIMIT_NUMBER_OF_SAMPLING_PORTS 512    /* partition scope    */
#define SYSTEM_LIMIT_NUMBER_OF_QUEUING_PORTS 512     /* partition scope    */

#define MAX_NAME_LENGTH 30

#define INFINITE_TIME_VALUE -1ll

/*----------------------*/
/* Base APEX types      */
/*----------------------*/
/* The actual size of these base types is system specific and the */
/* sizes must match the sizes used by the implementation of the */
/* underlying Operating System. */
typedef air_u8_t     APEX_BYTE;         /* 8-bit unsigned  */
typedef air_i32_t    APEX_INTEGER;      /* 32-bit signed   */
typedef air_u32_t    APEX_UNSIGNED;     /* 32-bit unsigned */
typedef air_i64_t    APEX_LONG_INTEGER; /* 64-bit signed   */

typedef air_name_t                           NAME_TYPE;
typedef void                                 (* SYSTEM_ADDRESS_TYPE);
typedef APEX_BYTE                            * MESSAGE_ADDR_TYPE;
typedef APEX_INTEGER                         MESSAGE_SIZE_TYPE;
typedef APEX_INTEGER                         MESSAGE_RANGE_TYPE;

typedef enum
{
    SOURCE                  = AIR_SOURCE_PORT,
    DESTINATION             = AIR_DESTINATION_PORT

} PORT_DIRECTION_TYPE;


typedef enum { FIFO = 0, PRIORITY = 1      } QUEUING_DISCIPLINE_TYPE;
typedef APEX_LONG_INTEGER                    SYSTEM_TIME_TYPE;


typedef NAME_TYPE     PROCESS_NAME_TYPE;
typedef APEX_INTEGER  PROCESS_ID_TYPE;
typedef APEX_INTEGER  LOCK_LEVEL_TYPE;
typedef APEX_UNSIGNED STACK_SIZE_TYPE;
typedef APEX_INTEGER  WAITING_RANGE_TYPE;
typedef APEX_INTEGER  PRIORITY_TYPE;

typedef enum {

	NO_ERROR                = AIR_NO_ERROR,          /* request valid and operation performed */
	NO_ACTION               = AIR_NO_ACTION,         /* status of system unaffected by request */
	NOT_AVAILABLE           = AIR_NOT_AVAILABLE,     /* resource required by request unavailable */
	INVALID_PARAM           = AIR_INVALID_PARAM,     /* invalid parameter specified in request */
	INVALID_CONFIG          = AIR_INVALID_CONFIG,    /* parameter incompatible with configuration */
	INVALID_MODE            = AIR_INVALID_MODE,      /* request incompatible with current mode */
	TIMED_OUT               = AIR_TIMED_OUT          /* time-out tied up with request has expired */

} RETURN_CODE_TYPE;

typedef enum {

	IDLE              = AIR_MODE_IDLE,
	COLD_START        = AIR_MODE_COLD_START,
	WARM_START        = AIR_MODE_WARM_START,
	NORMAL            = AIR_MODE_NORMAL

} OPERATING_MODE_TYPE;

typedef air_identifier_t PARTITION_ID_TYPE;

typedef enum {

	NORMAL_START            = AIR_START_CONDITION_NORMAL,
	PARTITION_RESTART       = AIR_START_CONDITION_PARTITION_RESTART,
	HM_MODULE_RESTART       = AIR_START_CONDITION_HM_MODULE_RESTART,
	HM_PARTITION_RESTART    = AIR_START_CONDITION_HM_PARTITION_RESTART
} START_CONDITION_TYPE;

typedef struct {

	SYSTEM_TIME_TYPE PERIOD;
	SYSTEM_TIME_TYPE DURATION;
	PARTITION_ID_TYPE IDENTIFIER;
	LOCK_LEVEL_TYPE LOCK_LEVEL;
	OPERATING_MODE_TYPE OPERATING_MODE;
	START_CONDITION_TYPE START_CONDITION;

}PARTITION_STATUS_TYPE;

extern void GET_PARTITION_STATUS ( PARTITION_STATUS_TYPE *PARTITION_STATUS,
								   RETURN_CODE_TYPE *RETURN_CODE );

extern void SET_PARTITION_MODE ( OPERATING_MODE_TYPE OPERATING_MODE,
								 RETURN_CODE_TYPE *RETURN_CODE );

extern void GET_TIME (
	/*out*/ SYSTEM_TIME_TYPE *SYSTEM_TIME,
	/*out*/ RETURN_CODE_TYPE *RETURN_CODE );

#define MAX_NUMBER_OF_SAMPLING_PORTS SYSTEM_LIMIT_NUMBER_OF_SAMPLING_PORTS

typedef APEX_BYTE MESSAGE_AREA_TYPE;

typedef NAME_TYPE SAMPLING_PORT_NAME_TYPE;

typedef APEX_INTEGER SAMPLING_PORT_ID_TYPE;

typedef enum {
    INVALID    = AIR_MESSAGE_INVALID,
    VALID      = AIR_MESSAGE_VALID
} VALIDITY_TYPE;

typedef struct {
	SYSTEM_TIME_TYPE REFRESH_PERIOD;
	MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE;
	PORT_DIRECTION_TYPE PORT_DIRECTION;
	VALIDITY_TYPE LAST_MSG_VALIDITY;
} SAMPLING_PORT_STATUS_TYPE;

extern void CREATE_SAMPLING_PORT (
	/*in */ SAMPLING_PORT_NAME_TYPE SAMPLING_PORT_NAME,
	/*in */ MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
	/*in */ PORT_DIRECTION_TYPE PORT_DIRECTION,
	/*in */ SYSTEM_TIME_TYPE REFRESH_PERIOD,
	/*out*/ SAMPLING_PORT_ID_TYPE *SAMPLING_PORT_ID,
	/*out*/ RETURN_CODE_TYPE *RETURN_CODE );

extern void WRITE_SAMPLING_MESSAGE (
	/*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
	/*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR, /* by reference */
	/*in */ MESSAGE_SIZE_TYPE LENGTH,
	/*out*/ RETURN_CODE_TYPE *RETURN_CODE );

extern void READ_SAMPLING_MESSAGE (
	/*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
	/*out*/ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
	/*out*/ MESSAGE_SIZE_TYPE *LENGTH,
	/*out*/ VALIDITY_TYPE *VALIDITY,
	/*out*/ RETURN_CODE_TYPE *RETURN_CODE );

extern void GET_SAMPLING_PORT_ID (
	/*in */ SAMPLING_PORT_NAME_TYPE SAMPLING_PORT_NAME,
	/*out*/ SAMPLING_PORT_ID_TYPE *SAMPLING_PORT_ID,
	/*out*/ RETURN_CODE_TYPE *RETURN_CODE );

extern void GET_SAMPLING_PORT_STATUS (
	/*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
	/*out*/ SAMPLING_PORT_STATUS_TYPE *SAMPLING_PORT_STATUS,
	/*out*/ RETURN_CODE_TYPE *RETURN_CODE );

#define MAX_NUMBER_OF_QUEUING_PORTS SYSTEM_LIMIT_NUMBER_OF_QUEUING_PORTS
typedef NAME_TYPE QUEUING_PORT_NAME_TYPE;
typedef APEX_INTEGER QUEUING_PORT_ID_TYPE;

typedef struct {
	MESSAGE_RANGE_TYPE NB_MESSAGE;
	MESSAGE_RANGE_TYPE MAX_NB_MESSAGE;
	MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE;
	PORT_DIRECTION_TYPE PORT_DIRECTION;
	WAITING_RANGE_TYPE WAITING_PROCESSES;
} QUEUING_PORT_STATUS_TYPE;

extern void CREATE_QUEUING_PORT (
	/*in */ QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME,
	/*in */ MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
	/*in */ MESSAGE_RANGE_TYPE MAX_NB_MESSAGE,
	/*in */ PORT_DIRECTION_TYPE PORT_DIRECTION,
	/*in */ QUEUING_DISCIPLINE_TYPE QUEUING_DISCIPLINE,
	/*out*/ QUEUING_PORT_ID_TYPE *QUEUING_PORT_ID,
	/*out*/ RETURN_CODE_TYPE *RETURN_CODE );

extern void SEND_QUEUING_MESSAGE (
	/*in */ QUEUING_PORT_ID_TYPE QUEUING_PORT_ID,
	/*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR, /* by reference */
	/*in */ MESSAGE_SIZE_TYPE LENGTH,
	/*in */ SYSTEM_TIME_TYPE TIME_OUT,
	/*out*/ RETURN_CODE_TYPE *RETURN_CODE);

extern void RECEIVE_QUEUING_MESSAGE (
	/*in */ QUEUING_PORT_ID_TYPE QUEUING_PORT_ID,
	/*in */ SYSTEM_TIME_TYPE TIME_OUT,
	/*out*/ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
	/*out*/ MESSAGE_SIZE_TYPE *LENGTH,
	/*out*/ RETURN_CODE_TYPE *RETURN_CODE );

extern void GET_QUEUING_PORT_ID (
	/*in */ QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME,
	/*out*/ QUEUING_PORT_ID_TYPE *QUEUING_PORT_ID,
	/*out*/ RETURN_CODE_TYPE *RETURN_CODE );

extern void GET_QUEUING_PORT_STATUS (
	/*in */ QUEUING_PORT_ID_TYPE QUEUING_PORT_ID,
	/*out*/ QUEUING_PORT_STATUS_TYPE *QUEUING_PORT_STATUS,
	/*out*/ RETURN_CODE_TYPE *RETURN_CODE );


/* HM types and definitions ------------------------------- */

#define MAX_ERROR_MESSAGE_SIZE 64
typedef APEX_INTEGER ERROR_MESSAGE_SIZE_TYPE;
typedef APEX_BYTE ERROR_MESSAGE_TYPE[MAX_ERROR_MESSAGE_SIZE];


typedef enum
{
	   PROCESS_ERROR 	= 0,	/* process_level error 		*/
	   PARTITON_ERROR 	= 1, 	/* partition_level error  	*/
	   MODULE_ERROR 	= 2,	/* module_level error  		*/
} ERROR_LEVEL_TYPE;

typedef enum
{
	DEADLINE_MISSED 	= 0,
	APPLICATION_ERROR 	= 1,
	NUMERIC_ERROR 		= 2,
	ILLEGAL_REQUEST 	= 3,
	STACK_OVERFLOW 		= 4,
	MEMORY_VIOLATION 	= 5,
	HARDWARE_FAULT 		= 6,
	POWER_FAIL 			= 7,
	PROCESS_EH_FAIL 	= 8
}ERROR_CODE_TYPE;


typedef struct
{
	ERROR_CODE_TYPE 		ERROR_CODE;
	ERROR_MESSAGE_TYPE 		MESSAGE;
	ERROR_MESSAGE_SIZE_TYPE LENGTH;
	SYSTEM_ADDRESS_TYPE 	FAILED_ADDRESS;
} ERROR_STATUS_TYPE;

extern void RAISE_APPLICATION_ERROR (
	/*in */ ERROR_CODE_TYPE 		ERROR_CODE,
	/*in */ MESSAGE_ADDR_TYPE 		MESSAGE_ADDR,
	/*in */ ERROR_MESSAGE_SIZE_TYPE LENGTH,
	/*out*/ RETURN_CODE_TYPE  		*RETURN_CODE );


/* MMS types and definitions ------------------------------- */

typedef APEX_INTEGER SCHEDULE_ID_TYPE;
typedef NAME_TYPE SCHEDULE_NAME_TYPE;

typedef struct {
	SYSTEM_TIME_TYPE TIME_OF_LAST_SCHEDULE_SWITCH;
	SCHEDULE_ID_TYPE CURRENT_SCHEDULE;
	SCHEDULE_ID_TYPE NEXT_SCHEDULE;
} SCHEDULE_STATUS_TYPE;


extern void SET_MODULE_SCHEDULE (
	/*in */ SCHEDULE_ID_TYPE SCHEDULE_ID,
	/*out*/ RETURN_CODE_TYPE *RETURN_CODE);

extern void GET_MODULE_SCHEDULE_STATUS (
	/*out*/ SCHEDULE_STATUS_TYPE *SCHEDULE_STATUS,
	/*out*/ RETURN_CODE_TYPE *RETURN_CODE);

extern void GET_MODULE_SCHEDULE_ID (
	/*in */ SCHEDULE_NAME_TYPE SCHEDULE_NAME,
	/*out*/ SCHEDULE_ID_TYPE *SCHEDULE_ID,
	/*out*/ RETURN_CODE_TYPE *RETURN_CODE);




/* Sampling Port Extensions constant and type definitions and
 * management services --------------------------------------- */

typedef enum {
	EMPTY_PORT 			= AIR_EMPTY_PORT,
	CONSUMED_MESSAGE 	= AIR_CONSUMED_MESSAGE,
	NEW_MESSAGE 		= AIR_NEW_MESSAGE
} UPDATED_TYPE;

typedef enum {
	STALE               = AIR_MESSAGE_STALE,
	FRESH               = AIR_MESSAGE_FRESH
} AGE_TYPE;

typedef struct {
	SYSTEM_TIME_TYPE 	REFRESH_PERIOD;
	SYSTEM_TIME_TYPE 	TIME_STAMP;
	MESSAGE_SIZE_TYPE 	MAX_MESSAGE_SIZE;
	PORT_DIRECTION_TYPE PORT_DIRECTION;
	AGE_TYPE			MESSAGE_AGE;
	UPDATED_TYPE 		UPDATED;
} SAMPLING_PORT_CURRENT_STATUS_TYPE;


extern void READ_UPDATED_SAMPLING_MESSAGE (
	/*in */ SAMPLING_PORT_ID_TYPE	SAMPLING_PORT_ID,
	/*in */ MESSAGE_ADDR_TYPE 		MESSAGE_ADDR,
	/*out*/ MESSAGE_SIZE_TYPE 		*LENGTH,
	/*out*/ UPDATED_TYPE			*UPDATED,
	/*out*/ RETURN_CODE_TYPE 		*RETURN_CODE);

extern void GET_SAMPLING_PORT_CURRENT_STATUS (
	/*in */ SAMPLING_PORT_ID_TYPE 				SAMPLING_PORT_ID,
	/*out*/ SAMPLING_PORT_CURRENT_STATUS_TYPE 	*SAMPLING_PORT_CURRENT_STATUS,
	/*out*/ RETURN_CODE_TYPE 					*RETURN_CODE);

extern void READ_SAMPLING_MESSAGE_CONDITIONAL (
	/*in */ SAMPLING_PORT_ID_TYPE	SAMPLING_PORT_ID,
	/*in */ SYSTEM_TIME_TYPE 		REF_TIME_STAMP,
	/*in */ MESSAGE_ADDR_TYPE 		MESSAGE_ADDR,
	/*out*/ MESSAGE_SIZE_TYPE 		*LENGTH,
	/*out*/ SYSTEM_TIME_TYPE 		*TIME_STAMP,
	/*out*/ RETURN_CODE_TYPE 		*RETURN_CODE);


#endif




