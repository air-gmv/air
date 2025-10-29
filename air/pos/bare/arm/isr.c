/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file isr.c
 * @author lumm
 * @brief POS exception handlers
 */

#include <air.h>
#include <bare.h>
#ifdef POS_DEBUG
#include <pprintf.h>
#endif

/**
 * @brief Core exception ISR tables
 */
extern volatile air_uptr_t *isr_table[PMK_MAX_CORES];

typedef void (*isr)(void);

void isr_install_handler(air_u32_t id, void *new, void **old) {

    /* get current core id */
    air_u32_t core_id = air_syscall_get_core_id();

    // In bare we currently want to have interrupts disabled
    // And only have aborts enabled
    air_u32_t level;
    air_syscall_disable_interrupts(level);
    air_syscall_disable_aborts();

    /* get old vector handle */
    *old = (void *)isr_table[core_id][id];
    isr_table[core_id][id] = (air_u32_t)new;
    
    air_syscall_enable_aborts();
}

void pos_hm_undef(pos_hm_stack *hm_stack){

    /* get current core id */
    air_u32_t core_id = air_syscall_get_core_id();

#ifdef POS_DEBUG
    pprintf("\n** POS UNDEFINED ERROR**\n");
#endif
    ((isr)isr_table[core_id][AIR_IRQ_HM_EVENT])();

}

void pos_hm_pref_abort(pos_hm_stack *hm_stack){

    /* get current core id */
    air_u32_t core_id = air_syscall_get_core_id();

#ifdef POS_DEBUG
    pprintf("\n** POS PREFETCH ERROR**\n");
#endif
    ((isr)isr_table[core_id][AIR_IRQ_HM_EVENT])();
}

void pos_hm_data_abort(pos_hm_stack *hm_stack){

    /* get current core id */
    air_u32_t core_id = air_syscall_get_core_id();

#ifdef POS_DEBUG
    pprintf("\n** POS DATA ERROR**\n");
#endif
    ((isr)isr_table[core_id][AIR_IRQ_HM_EVENT])();
}
