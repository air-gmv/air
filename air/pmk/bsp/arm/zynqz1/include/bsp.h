/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file bsp.h
 * @author lumm
 * @brief bsp headers and inline assembly functions.
 */

#ifndef ARM_BSP_H
#define ARM_BSP_H

#ifndef ASM

#define SCU_BASE_MEMORY     0xf8f00000
#define IC_CPU_BASE_MEMORY  0xf8f00100
#define IC_DIST_BASE_MEMORY 0xf8f01000

#include <air_arch.h>
#include <armv7.h>
#include <arm_a9mpcore.h>
#include <arm_cp15.h>
#include <isr.h>
#include <svc.h>
#include <mmu.h>
#include <bsp_segregation.h>
#include <gic.h>
#include <slcr.h>
#include <timer.h>
#include <uart.h>

void pmk_trap_table(void);
void air_kernel_bss_start(void);
void air_kernel_bss_end(void);

#define BSP_IPC_IRQ     ARM_GIC_IRQ_SGI_14
#define BSP_IPC_PCS     ARM_GIC_IRQ_SGI_15

void bsp_start_hook(void);
air_u32_t bsp_core_init(void);
void bsp_core_ready(void);
void bsp_boot_core(air_u32_t cpu_id, void *entry_point);
void bsp_restart_core(pmk_core_ctrl_t *core_ctx);
void bsp_shutdown_core(pmk_core_ctrl_t *core_ctx);
void bsp_interrupt_broadcast(air_u32_t dummy);

#define bsp_get_core_id() arm_cp15_get_multiprocessor_cpu_id()
#define bsp_get_number_of_cores() arm_ic_processor_count()

static inline void bsp_interrupt_core(air_u32_t core, air_u32_t dummy) {
    if (bsp_get_core_id() == core) {
        __asm__ volatile ("wfi");
    }
}

void bsp_idle_loop(void);


static inline void bsp_wait_for_event(void) {
    arm_data_synchronization_barrier(15);
    __asm__ volatile ("wfe" : : : "memory");
}

static inline void bsp_send_event(void) {
    __asm__ volatile ("sev" : : : "memory");
}

static inline void bsp_clear_bss(void) {
    air_u32_t bss_size = (air_u32_t)air_kernel_bss_end - (air_u32_t)air_kernel_bss_start;

    memset(air_kernel_bss_start, 0, bss_size);

    return;
}


//static inline void copy_vector_table() {
//  memcpy(bsp_vector_table_begin, vector_table_begin, 8);
//  if (vector_table_begin != vector_table_start) {
//      air_i8_t *dst = (air_i8_t *) bsp_vector_table_begin;
//      air_i8_t *src = (air_i8_t *) vector_table_begin;
//
//      for(air_u32_t i = 0; i < 8*4; i++) {
//          *dst++ = *src++;
//      }
//  }
//}

static inline void arm_set_vector_base(void) {

    if ((air_u32_t)pmk_trap_table != 0) {
        air_u32_t ctrl;

        /* TODO Assumes every core has Security Extensions */
        arm_cp15_set_vector_base_address((void *)pmk_trap_table);

        ctrl = arm_cp15_get_system_control();
        ctrl &= ~ARM_SCTLR_V;
        arm_cp15_set_system_control(ctrl);
    }
}

#define bsp_console_outbyte(ch) arm_uart_transmit(ch)

#endif /* ASM */

#endif /* ARM_BSP_H */
