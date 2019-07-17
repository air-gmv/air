/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013 - 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file segregation.h
 * @author lumm
 * @brief BSP segregation
 */

#ifndef ARM_SEGREGATION_H
#define ARM_SEGREGATION_H

#include <partition.h>

void bsp_segregation(pmk_partition_t *partition);

#endif /* ARM_SEGREGATION_H */
