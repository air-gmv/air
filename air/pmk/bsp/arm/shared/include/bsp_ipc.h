/*
 * Copyright (C) 2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file bsp_ipc.h
 * \author lumm
 * \brief Inter-processor interrupts
 */

#ifndef BSP_IPC_H_
#define BSP_IPC_H_

#include <a9mpcore.h>
#include <gic.h>

#define BSP_IPC_IRQ     ARM_GIC_IRQ_SGI_14
#define BSP_IPC_PCS     ARM_GIC_IRQ_SGI_15

void arm_setup_ipc(air_u32_t cpu_id);

#endif /* BSP_IPC_H_ */
