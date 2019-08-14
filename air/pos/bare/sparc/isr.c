/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */

#include <air.h>
#include <cpu.h>
#include <configurations.h>

/**
 * @brief System trap table
 */
extern int bare_trap_table;
/**
 * @brief Core trap ISR tables
 */
extern volatile air_u32_t *isr_table[PMK_MAX_CORES];
/**
 * @brief Secondary core ISR handler (defined in rtems_smp_asm.S)
 */
void sparc_isr_handler();

void isr_install_raw_handler(air_u32_t tn, void *new, void **old) {

    /* get real trap number */
    air_u32_t rtn = SPARC_REAL_TRAP_NUMBER(tn);

    /* get TBR vector */
    air_u32_t *tbr = (void *)&bare_trap_table;

    /* fetch old handler and apply new handler */
    air_sparc_disable_traps();
    *old = (void *)tbr[rtn];
    tbr[rtn] = (air_u32_t)new;
    air_sparc_enable_traps();
}

void isr_install_handler(air_u32_t tn, void *new, void **old) {

    /* get current core id */
    air_u32_t core_id = air_syscall_get_core_id();

    /* get real trap number */
    air_u32_t rtn = SPARC_REAL_TRAP_NUMBER(tn);

    /* get old vector handle */
    air_sparc_disable_traps();
    *old = (void *)isr_table[core_id][rtn];
    air_sparc_enable_traps();

    /* set the raw handler */
    void *ignored;
    isr_install_raw_handler(rtn, sparc_isr_handler, &ignored);

    /* set the new vector handler */
    air_sparc_disable_traps();
    isr_table[core_id][rtn] = (air_u32_t)new;
    air_sparc_enable_traps();
}
