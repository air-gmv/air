/*
 * Copyright (C) 2018  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file bsp_segregation.h
 * \author lumm
 * \brief BSP segregation
 */

#ifndef BSP_SEGREGATION_H_
#define BSP_SEGREGATION_H_

#include <partition.h>

void bsp_segregation(pmk_partition_t *partition);

/**
 * \brief Saves the core context
 * \param core_ctrl control control pointer
 */
void arm_core_context_save(pmk_core_ctrl_t *core);

/**
 * \brief Restores the core context
 * \param core_ctrl control control pointer
 */
void arm_core_context_restore(pmk_core_ctrl_t *core);

#endif /* BSP_SEGREGATION_H_ */
