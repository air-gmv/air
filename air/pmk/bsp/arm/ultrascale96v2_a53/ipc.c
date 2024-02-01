/*
 * Copyright (C) 2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file ipc.c
 * \author lumm
 * \brief Inter-processor interrupts
 */

#include <bsp_ipc.h>
#include <isr.h>
#include <gic.h>
#include <workspace.h>

void arm_setup_ipc(air_u32_t cpu_id) {

    if (cpu_id == 0) {
        arm_isr_install_handler(ARM_A53_IRQ_SPT, pmk_partition_scheduler);
        arm_int_set_priority(ARM_A53_IRQ_SPT, 0);
        
        arm_isr_install_handler(BSP_IPC_IRQ, pmk_partition_scheduler);
        arm_isr_install_handler(BSP_IPC_PCS, pmk_ipc_handler);
    }

    arm_int_set_priority(BSP_IPC_IRQ, 0);
    arm_int_set_priority(BSP_IPC_PCS, 0);
}
