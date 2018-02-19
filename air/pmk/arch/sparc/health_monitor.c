/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file sparc_health_monitor.c
 * @author pfnf
 * @brief 
 */

#include <pmk.h>
#include <sparc.h>
#include <multicore.h>
#include <health_monitor.h>
#include <printk.h>

/**
 * @brief SPARC Error mapping structure
 */
typedef struct {

    air_error_e error_id;
    char*           error_str;

} sparc_error_map_t;


air_error_e error_map[] = {

    AIR_POWER_ERROR,            /* Power-on reset                             */
    AIR_SEGMENTATION_ERROR,     /* Error during instruction fetch             */
    AIR_UNIMPLEMENTED_ERROR,    /* UNIMP or other un-implemented instruction  */
    AIR_VIOLATION_ERROR,        /* Privileged instruction in user mode        */
    AIR_FLOAT_ERROR,            /* FP instruction while FPU disabled          */
    AIR_SEGMENTATION_ERROR,     /* SAVE into invalid window                   */
    AIR_SEGMENTATION_ERROR,     /* RESTORE into invalid window                */
    AIR_SEGMENTATION_ERROR,     /* Memory access to un-aligned address        */
    AIR_FLOAT_ERROR,            /* FPU exception                              */
    AIR_SEGMENTATION_ERROR,     /* Access error during load/store instruction */
    AIR_ARITHMETIC_ERROR,       /* Tagged arithmetic overflow                 */
    AIR_UNIMPLEMENTED_ERROR,    /* Hardware breakpoint match                  */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_IO_ERROR,               /* Asynchronous interrupt 1                   */
    AIR_IO_ERROR,               /* Asynchronous interrupt 2                   */
    AIR_IO_ERROR,               /* Asynchronous interrupt 3                   */
    AIR_IO_ERROR,               /* Asynchronous interrupt 4                   */
    AIR_IO_ERROR,               /* Asynchronous interrupt 5                   */
    AIR_IO_ERROR,               /* Asynchronous interrupt 6                   */
    AIR_IO_ERROR,               /* Asynchronous interrupt 7                   */
    AIR_IO_ERROR,               /* Asynchronous interrupt 8                   */
    AIR_IO_ERROR,               /* Asynchronous interrupt 9                   */
    AIR_IO_ERROR,               /* Asynchronous interrupt 10                  */
    AIR_IO_ERROR,               /* Asynchronous interrupt 11                  */
    AIR_IO_ERROR,               /* Asynchronous interrupt 12                  */
    AIR_IO_ERROR,               /* Asynchronous interrupt 13                  */
    AIR_IO_ERROR,               /* Asynchronous interrupt 14                  */
    AIR_IO_ERROR,               /* Asynchronous interrupt 15                  */
    AIR_IO_ERROR,               /* register file EDAC error                   */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_UNIMPLEMENTED_ERROR,    /* CP instruction while Co-processor disabled */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_IO_ERROR,               /* Co-processor exception                     */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_DIVISION_BY_0_ERROR,    /* Divide by zero                             */
    AIR_SEGMENTATION_ERROR,     /* write buffer error                         */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* GDB breakpoint trap                        */
    AIR_DIVISION_BY_0_ERROR,    /* SW Divide by zero                          */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* APEX Error                                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
    AIR_VIOLATION_ERROR,        /* Unexpected/Unassigned trap                 */
};

/**
 * @brief SPARC internal Health-Monitor handler
 * @param isf interrupt stack frame pointer
 * @param core_ctrl core control structure pointer
 */
void sparc_hm_handler(
        sparc_interrupt_stack_frame_t *isf, pmk_core_ctrl_t *core_ctrl) {

    /* map it to the PMK independent data */
    air_error_e error_id = error_map[isf->tpc];

    /* call the PMK health monitor */
    printk ("HM error detected id - %d\n", error_id);
    pmk_hm_isr_handler(error_id);

    /* jump the current instruction */
    isf->pc   = isf->nkpc;
    isf->nkpc = isf->nkpc + 4;
}

/**
 * @brief Health-Monitor architecture dependent Initialization
 *
 * This function should be use to register architecture dependent traps and
 * interrupts to handle the errors caused by the PMK and the Partitions
 */
void sparc_healthmonitor_init() {

    /*
     * 0x01 - instruction access exception
     * 0x02 - illegal instruction
     * 0x03 - privileged instruction
     * 0x04 - fp disabled
     * 0x07 - memory address not aligned
     * 0x08 - fp exception
     * 0x0A - tag overflow
     * 0x0B - watchpoint_detected
     */
    sparc_install_vector_trap_handler(0x01, sparc_hm_handler);
    sparc_install_vector_trap_handler(0x02, sparc_hm_handler);
    sparc_install_vector_trap_handler(0x03, sparc_hm_handler);
    sparc_install_vector_trap_handler(0x04, sparc_hm_handler);
    sparc_install_vector_trap_handler(0x07, sparc_hm_handler);
    sparc_install_vector_trap_handler(0x08, sparc_hm_handler);
    sparc_install_vector_trap_handler(0x09, sparc_hm_handler);
    sparc_install_vector_trap_handler(0x0A, sparc_hm_handler);
    sparc_install_vector_trap_handler(0x0B, sparc_hm_handler);

    /*
     * 0x20 r_register_access_error
     * 0x21 instruction_access_error
     */
    sparc_install_vector_trap_handler(0x20, sparc_hm_handler);
    sparc_install_vector_trap_handler(0x21, sparc_hm_handler);

    /*
     * 0x24 cp_disabled
     * 0x25 unimplemented_FLUSH
     */
    sparc_install_vector_trap_handler(0x24, sparc_hm_handler);
    sparc_install_vector_trap_handler(0x25, sparc_hm_handler);

    /*
     * 0x28 cp_exception
     * 0x29 data_access_error
     * 0x2A division_by_zero
     * 0x2B data_store_error
     * 0x2C data_access_MMU_miss
     */
    sparc_install_vector_trap_handler(0x28, sparc_hm_handler);
    sparc_install_vector_trap_handler(0x29, sparc_hm_handler);
    sparc_install_vector_trap_handler(0x2A, sparc_hm_handler);
    sparc_install_vector_trap_handler(0x2B, sparc_hm_handler);
    sparc_install_vector_trap_handler(0x2C, sparc_hm_handler);

    /* 0x3C instruction_access_MMU_miss */
    sparc_install_vector_trap_handler(0x3C, sparc_hm_handler);

    /* 0xF0 raise application error */
    sparc_install_vector_trap_handler(0xF0, sparc_hm_handler);
}
