/*
 * Copyright (C) 2008-2012  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author cdcs
 * @date 2011
 *
 * @brief IMASPEX services and definitions
 *
 */

#ifndef _IMASPEX_H
#define _IMASPEX_H

#include <a653.h>
#include <air.h>

/**
 * @brief The imaspex entry point
 */
int imaspex_init(void);

/**
 * @name CACHE TYPES
 * @{
 *
 *  IMASPEX cache services receive the cache type as input argument.
 *  The available options are: data cache, instruction cache or both.
 */

typedef enum
{

    /** Instruction and Data cache          */
    ALL_CACHES = AIR_CACHE_ALL,
    /** Data cache only                     */
    DATA_CACHE = AIR_CACHE_DATA,
    /** Instruction cache only              */
    INSTRUCTION_CACHE = AIR_CACHE_INSTRUCTION,

} CACHE_TYPE;

/** @} */

/**
 * @param [in] CACHE CACHE TYPE
 * @param [out] Status Code
 * @brief flush cache
 *
 * This service allows an application in a Partition to flush the
 * content of the Cache at any time. When cache has been disabled at
 * configuration level, this service can not be used.
 */
void FLUSH_CACHE(CACHE_TYPE CACHE, RETURN_CODE_TYPE *RETURN_CODE);

/**
 * @param [in] CACHE CACHE TYPE
 * @param [out] Status Code
 * @brief activate cache
 *
 * This service is used to activate the cache for the current partition when
 * cache is enabled at configuration level but has been deactivated with the
 * CACHE_DEACTIVATION service. When cache has been disabled at configuration
 * level, this service can not be used.
 */
void ACTIVATE_CACHE(CACHE_TYPE CACHE, RETURN_CODE_TYPE *RETURN_CODE);

/**
 * @param [in] CACHE CACHE TYPE
 * @param [out] Status Code
 * @brief deactivate cache
 *
 * This service is used to deactivate the cache for the current partition when
 * cache is enabled at configuration level. When cache has been disabled at
 * configuration level, this service can not be used.
 */
void DEACTIVATE_CACHE(CACHE_TYPE CACHE, RETURN_CODE_TYPE *RETURN_CODE);

/**
 * @param [in] CACHE CACHE TYPE
 * @param [out] Status Code
 * @brief freeze cache
 *
 * This service is used to freeze the cache for the current partition when
 * cache is enabled at configuration level. When cache has been disabled at
 * configuration level, this service can not be used.
 */
void FREEZE_CACHE(CACHE_TYPE CACHE, RETURN_CODE_TYPE *RETURN_CODE);

/**
 * @param [in]	PARTITION_IDENTIFIER	the target partition ID
 * @param [in]	OPERATING_MODE			the new operating mode
 * @param [out]	RETURN_CODE				ARINC 653 return code
 *
 * Specified by IMASP-D10 - the SET_A_PARTITION_MODE extends the A653 service
 * SET_PARTITION_MODE: an application in a System Partition shall be able
 * to change the mode of any other partition.
 * \note order of parameters has not been specified
 */
void SET_A_PARTITION_MODE(PARTITION_ID_TYPE PARTITION_IDENTIFIER, OPERATING_MODE_TYPE OPERATING_MODE,
                          RETURN_CODE_TYPE *RETURN_CODE);

/**
 * @param [in]	PARTITION_IDENTIFIER	the target partition ID
 * @param [out]	PARTITION_STATUS		the partition status
 * @param [out]	RETURN_CODE				ARINC 653 return code
 *
 * Specified by IMASP-D10 - the GET_A_PARTITION_STATUS extends the A653 service
 * GET_PARTITION_STATUS: an application in a System Partition shall be able
 * to obtain the status of any other partition.
 * \note 	order of parameters has not been specified
 * \note 	if the specified partition is not part of the current module
 * 			schedule (it may take part in other module schedules) the values of
 * 			period and duration are set to 0 (zero).
 */
void GET_A_PARTITION_STATUS(PARTITION_ID_TYPE PARTITION_IDENTIFIER, PARTITION_STATUS_TYPE *PARTITION_STATUS,
                            RETURN_CODE_TYPE *RETURN_CODE);

/**
 * @param [in]	PARTITION_NAME		the target partition name
 * @param [out]	PARTITION_ID		the partition id
 * @param [out]	RETURN_CODE			ARINC 653 return code
 *
 * Specified by IMASP-D10 - GET_A_PARTITION_ID obtains the identifier of the
 * specified partition through its name.
 */
void GET_A_PARTITION_ID(NAME_TYPE PARTITION_NAME, PARTITION_ID_TYPE *PARTITION_ID, RETURN_CODE_TYPE *RETURN_CODE);

/**
 * @param [out]	PARTITION_ID		the calling partition id
 * @param [out]	RETURN_CODE			ARINC 653 return code
 *
 * Specified by IMASP-D10 - GET_PARTITION_ID the identifier of the partition
 * performing the request.
 */
void GET_PARTITION_ID(PARTITION_ID_TYPE *PARTITION_ID, RETURN_CODE_TYPE *RETURN_CODE);

/**
 * @param [out]	RETURN_CODE			ARINC 653 return code
 *
 * Specified by IMASP-D10 - SIGNAL_ACTIVITY_COMPLETION
 * \brief Used to signal the end of critical tasks in the current execution
 * 		  window
 */
void SIGNAL_ACTIVITY_COMPLETION(RETURN_CODE_TYPE *RETURN_CODE);

/**
 * \brief  an application in a Partition shall be able to obtain the status of
 * its partition.
 *
 * \param [out] PARTITION_STATUS        the partition status
 * \param [out] RETURN_CODE             ARINC 653 return code
 */
void GET_PARTITION_STATUS(PARTITION_STATUS_TYPE *PARTITION_STATUS, RETURN_CODE_TYPE *RETURN_CODE);

/**
 * \brief an application in a Partition shall be able to change the mode of
 * its partition.
 *
 * \param [in]  OPERATING_MODE          the new operating mode
 * \param [out] RETURN_CODE             ARINC 653 return code
 */
void SET_PARTITION_MODE(OPERATING_MODE_TYPE OPERATING_MODE, RETURN_CODE_TYPE *RETURN_CODE);

/**
 * @brief Obtain the current local node time
 * @note obtained time is monotonic since boot.
 * @param [out] SYSTEM_TIME current time with nanosecond precision
 * @param [out] RETURN_CODE ARINC 653 return code
 */
void GET_TIME(SYSTEM_TIME_TYPE *SYSTEM_TIME, RETURN_CODE_TYPE *RETURN_CODE);

/**
 * @param [in]  PARTITION_IDENTIFIER    the target partition ID
 * @param [in]  OPERATING_MODE          the new operating mode
 * @param [out] RETURN_CODE             ARINC 653 return code
 *
 * Specified by IMASP-D10 - the SET_A_PARTITION_MODE extends the A653 service
 * SET_PARTITION_MODE: an application in a System Partition shall be able
 * to change the mode of any other partition.
 * \note order of parameters has not been specified
 */
void SET_A_PARTITION_MODE(PARTITION_ID_TYPE PARTITION_IDENTIFIER, OPERATING_MODE_TYPE OPERATING_MODE,
                          RETURN_CODE_TYPE *RETURN_CODE);

/**
 * @param [in]  PARTITION_IDENTIFIER    the target partition ID
 * @param [out] PARTITION_STATUS        the partition status
 * @param [out] RETURN_CODE             ARINC 653 return code
 *
 * Specified by IMASP-D10 - the GET_A_PARTITION_STATUS extends the A653 service
 * GET_PARTITION_STATUS: an application in a System Partition shall be able
 * to obtain the status of any other partition.
 * \note    order of parameters has not been specified
 * \note    if the specified partition is not part of the current module
 *          schedule (it may take part in other module schedules) the values of
 *          period and duration are set to 0 (zero).
 */
void GET_A_PARTITION_STATUS(PARTITION_ID_TYPE PARTITION_IDENTIFIER, PARTITION_STATUS_TYPE *PARTITION_STATUS,
                            RETURN_CODE_TYPE *RETURN_CODE);

/**
 * @param [in]  PARTITION_NAME      the target partition name
 * @param [out] PARTITION_ID        the partition id
 * @param [out] RETURN_CODE         ARINC 653 return code
 *
 * Specified by IMASP-D10 - GET_A_PARTITION_ID obtains the identifier of the
 * specified partition through its name.
 */
void GET_A_PARTITION_ID(NAME_TYPE PARTITION_NAME, PARTITION_ID_TYPE *PARTITION_ID, RETURN_CODE_TYPE *RETURN_CODE);

/**
 * @param [out] PARTITION_ID        the calling partition id
 * @param [out] RETURN_CODE         ARINC 653 return code
 *
 * Specified by IMASP-D10 - GET_PARTITION_ID the identifier of the partition
 * performing the request.
 */
void GET_PARTITION_ID(PARTITION_ID_TYPE *PARTITION_ID, RETURN_CODE_TYPE *RETURN_CODE);

/**
 * @param [out] RETURN_CODE         ARINC 653 return code
 *
 * Specified by IMASP-D10 - SIGNAL_ACTIVITY_COMPLETION
 * \brief Used to signal the end of critical tasks in the current execution
 *        window
 */
void SIGNAL_ACTIVITY_COMPLETION(RETURN_CODE_TYPE *RETURN_CODE);

/* ARINC 653 (PART 1) SAMPLING PORT SERVICES */
/* -------------------------------------------------------------------------- */
void CREATE_SAMPLING_PORT(
    /*in */ SAMPLING_PORT_NAME_TYPE SAMPLING_PORT_NAME,
    /*in */ MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
    /*in */ PORT_DIRECTION_TYPE PORT_DIRECTION,
    /*in */ SYSTEM_TIME_TYPE REFRESH_PERIOD,
    /*out*/ SAMPLING_PORT_ID_TYPE *SAMPLING_PORT_ID,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE);

void WRITE_SAMPLING_MESSAGE(
    /*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
    /*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR, /* by reference */
    /*in */ MESSAGE_SIZE_TYPE LENGTH,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE);

void READ_SAMPLING_MESSAGE(
    /*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
    /*out*/ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
    /*out*/ MESSAGE_SIZE_TYPE *LENGTH,
    /*out*/ VALIDITY_TYPE *VALIDITY,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE);

void GET_SAMPLING_PORT_ID(
    /*in */ SAMPLING_PORT_NAME_TYPE SAMPLING_PORT_NAME,
    /*out*/ SAMPLING_PORT_ID_TYPE *SAMPLING_PORT_ID,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE);

void GET_SAMPLING_PORT_STATUS(
    /*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
    /*out*/ SAMPLING_PORT_STATUS_TYPE *SAMPLING_PORT_STATUS,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE);

/* ARINC 653 (PART 2) SAMPLING PORT SERVICES */
/* -------------------------------------------------------------------------- */
void READ_UPDATED_SAMPLING_MESSAGE(
    /*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
    /*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
    /*out*/ MESSAGE_SIZE_TYPE *LENGTH,
    /*out*/ UPDATED_TYPE *UPDATED,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE);

void GET_SAMPLING_PORT_CURRENT_STATUS(
    /*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
    /*out*/ SAMPLING_PORT_CURRENT_STATUS_TYPE *SAMPLING_PORT_CURRENT_STATUS,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE);

void READ_SAMPLING_MESSAGE_CONDITIONAL(
    /*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
    /*in */ SYSTEM_TIME_TYPE REF_TIME_STAMP,
    /*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
    /*out*/ MESSAGE_SIZE_TYPE *LENGTH,
    /*out*/ SYSTEM_TIME_TYPE *TIME_STAMP,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE);

void CREATE_QUEUING_PORT(
    /*in */ QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME,
    /*in */ MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
    /*in */ MESSAGE_RANGE_TYPE MAX_NB_MESSAGE,
    /*in */ PORT_DIRECTION_TYPE PORT_DIRECTION,
    /*in */ QUEUING_DISCIPLINE_TYPE QUEUING_DISCIPLINE,
    /*out*/ QUEUING_PORT_ID_TYPE *QUEUING_PORT_ID,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE);

void SEND_QUEUING_MESSAGE(
    /*in */ QUEUING_PORT_ID_TYPE QUEUING_PORT_ID,
    /*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR, /* by reference */
    /*in */ MESSAGE_SIZE_TYPE LENGTH,
    /*in */ SYSTEM_TIME_TYPE TIME_OUT,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE);

void RECEIVE_QUEUING_MESSAGE(
    /*in */ QUEUING_PORT_ID_TYPE QUEUING_PORT_ID,
    /*in */ SYSTEM_TIME_TYPE TIME_OUT,
    /*out*/ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
    /*out*/ MESSAGE_SIZE_TYPE *LENGTH,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE);

void GET_QUEUING_PORT_ID(
    /*in */ QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME,
    /*out*/ QUEUING_PORT_ID_TYPE *QUEUING_PORT_ID,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE);

void GET_QUEUING_PORT_STATUS(
    /*in */ QUEUING_PORT_ID_TYPE QUEUING_PORT_ID,
    /*out*/ QUEUING_PORT_STATUS_TYPE *QUEUING_PORT_STATUS,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE);

#endif /* _IMASPEX_H */
