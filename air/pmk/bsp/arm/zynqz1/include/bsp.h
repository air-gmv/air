/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file bsp.h
 * \author lumm
 * \brief bsp headers and inline assembly functions.
 */

#ifndef BSP_H_
#define BSP_H_

#ifndef ASM

#include <a9mpcore.h>
#include <workspace.h>
#include <gic.h>
#include <uart.h>
#include <bsp_segregation.h> // compatibility for segregation.c
#include <bsp_ipc.h> // compatibility for segregation.c

void pmk_trap_table(void);
void air_kernel_bss_start(void);
void air_kernel_bss_end(void);

void bsp_start_hook(void);
air_u32_t bsp_core_init(void);
void bsp_core_ready(void);
void bsp_boot_core(air_u32_t cpu_id, void *entry_point);
void bsp_restart_core(pmk_core_ctrl_t *core_ctx);
void bsp_shutdown_core(pmk_core_ctrl_t *core_ctx);
void bsp_interrupt_broadcast(air_u32_t dummy);
void bsp_idle_loop(void);

#define bsp_get_core_id() arm_cp15_get_multiprocessor_cpu_id()
#define bsp_get_number_of_cores() arm_ic_processor_count()

static inline void bsp_interrupt_core(air_u32_t core, air_u32_t dummy) {
    if (bsp_get_core_id() == core) {
        __asm__ volatile ("wfi");
    }
}

static inline void bsp_wait_for_event(void) {
    arm_data_synchronization_barrier();
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
