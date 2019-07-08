/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file bsp.h
 * @author lumm
 * @brief BSP headers and inline assembly functions.
 */

#ifndef ARM_BSP_H
#define ARM_BSP_H

#ifndef ASM

#define SCU_BASE_MEMORY     0xf8f00000
#define GT_BASE_MEMORY      0xf8f00200
#define IC_CPU_BASE_MEMORY  0xf8f00100
#define IC_DIST_BASE_MEMORY 0xf8f01000

#include <air_arch.h>
#include <armv7.h>
#include <arm_cp15.h>
#include <gic.h>

extern air_uptr_t bsp_section_bss_start;
extern air_u32_t bsp_section_bss_size;
extern air_uptr_t bsp_vector_table_begin;
extern air_uptr_t bsp_vector_table_end;
extern air_u32_t bsp_vector_table_size;
extern air_uptr_t vector_table_begin;

void bsp_start_hook(void *vector_table_begin);

#define bsp_get_core_id() cp15_get_multiprocessor_cpu_id()
#define bsp_get_number_of_cores() ic_processor_count()

static inline void bsp_interrupt_core(air_u32_t core, air_u32_t dummy) {
    if (bsp_get_core_id() == core) {
        __asm__ volatile ("wfi");
    }
}

static inline void bsp_interrupt_broadcast(air_u32_t dummy) {
}

static inline void clear_bss() {
//  memset(bsp_section_bss_start, 0, bsp_section_bss_size);
    air_i8_t *mem = (air_i8_t *)bsp_section_bss_start;
    for(air_u32_t i = 0; i < bsp_section_bss_size; ++i) {
        *mem++ = 0;
    }
}

static inline void copy_vector_table() {
//  memcpy(bsp_vector_table_begin, vector_table_begin, 8);
    if (vector_table_begin != bsp_vector_table_begin) {
       air_i8_t *dst = (air_i8_t *) bsp_vector_table_begin;
       air_i8_t *src = (air_i8_t *) vector_table_begin;

       for(air_u32_t i = 0; i < 8; i++) {
           *dst++ = *src++;
       }
    }
}

static inline void set_vector_base(void) {
    /* Do not use bsp_vector_table_begin == 0, since this will get optimized
    * away. source: rtems source code
    */
    if (bsp_vector_table_end != bsp_vector_table_size) {
        air_u32_t ctrl;

        /* Assumes every core has Security Extensions */
        cp15_set_vector_base_address((void *)bsp_vector_table_begin);

        ctrl = cp15_get_system_control();
        ctrl &= ~CP15_CTRL_V;
        cp15_set_system_control(ctrl);
    }
}

#endif /* ASM */

#define BSP_IPC_IRQ 0 /* TODO for compatibility */
#define BSP_IPC_PCS 0 /* TODO for compatibility */

#endif /* ARM_BSP_H */
