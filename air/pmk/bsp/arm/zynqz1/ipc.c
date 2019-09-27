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
        arm_isr_install_handler(ZYNQ_IRQ_TTC_0_0, pmk_partition_scheduler);
        arm_isr_install_handler(ZYNQ_IRQ_TTC_0_1, pmk_partition_scheduler);
        arm_isr_install_handler(ZYNQ_IRQ_TTC_0_2, pmk_partition_scheduler);
        arm_isr_install_handler(BSP_IPC_IRQ, pmk_partition_scheduler);
        arm_isr_install_handler(BSP_IPC_PCS, pmk_ipc_handler);
    }

    arm_int_set_priority(ZYNQ_IRQ_TTC_0_0, 0);
    arm_int_set_priority(ZYNQ_IRQ_TTC_0_1, 0);
    arm_int_set_priority(ZYNQ_IRQ_TTC_0_2, 0);
    arm_int_set_priority(BSP_IPC_IRQ, 0);
    arm_int_set_priority(BSP_IPC_PCS, 0);

    arm_int_enable(ZYNQ_IRQ_TTC_0_0);
    arm_int_enable(ZYNQ_IRQ_TTC_0_1);
    arm_int_enable(ZYNQ_IRQ_TTC_0_2);
    arm_int_enable(BSP_IPC_IRQ);
    arm_int_enable(BSP_IPC_PCS);
}
