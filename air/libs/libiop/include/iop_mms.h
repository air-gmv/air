/*
 * Copyright (C) 2008-2013  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief
 */

#ifndef __MMS_H__
#define __MMS_H__

#include <iop.h>

/**
 * @brief MMS
 */
void iop_init_mms(void);
void iop_task_sleep(uint32_t);
void iop_change_schedule(void);

/**
 * @brief Current schedule status
 */
extern air_schedule_status_t air_schedule;

/**
 * @brief Current partition status
 */
extern air_partition_status_t air_partition;

#endif /* __MMS_H__ */
