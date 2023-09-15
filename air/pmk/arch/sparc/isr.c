/*
 * Copyright (C) 2013-2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief
 */

#include <sparc.h>

/**
 * @brief SPARC trap table entry
 */
typedef struct {

    air_u32_t inst_1;        /**< trap instruction 1             */
    air_u32_t inst_2;        /**< trap instruction 2             */
    air_u32_t inst_3;        /**< trap instruction 3             */
    air_u32_t inst_4;        /**< trap instruction 4             */

} sparc_trap_table_entry;

/**
 *  @brief Trap table entry (based on TRAP_HANDLER(n, handler) macro)
 */
const static sparc_trap_table_entry slot_template =
    {
        0xa1480000,    /* mov       %psr, %l0                   */
        0x29000000,    /* sethi     %hi(handler), %l4           */
        0x81c52000,    /* jmp       %l4 + %lo(handler)          */
        0xa6102000     /* mov       n, %l3                      */
    };

/**
 * @brief PMK Trap table
 */
extern int pmk_trap_table;

/**
 * @brief Assembly ISR handler
 */
extern void sparc_isr_handler_entry();

/**
 * @brief ISR Handler vector
 */
extern air_u32_t sparc_isr_vector[256];


void *sparc_install_raw_trap_handler(air_u32_t n, void *handler) {

    void *old_handler = NULL;

    /* get current trap table slot */
    sparc_trap_table_entry *slot = &((sparc_trap_table_entry *)&pmk_trap_table)[n];

    /* extract old handler if it match the template */
    if (slot->inst_1 == slot_template.inst_1) {
        old_handler = (void *)((slot->inst_2 << 10) | (slot->inst_3 & 0x000003FF));
    }

    /* setup the trap entry code  */
    *slot = slot_template;
    slot->inst_4 |= n;
    slot->inst_3 |= ((air_uptr_t)handler & 0x000003FF);
    slot->inst_2 |= ((air_uptr_t)handler & 0xFFFFFC00) >> 10;

    /* flush cache */
    __asm__ __volatile__("flush");
    return old_handler;
}

void *sparc_register_vector_trap_handler(air_u32_t n, void *handler) {

    void *old_handler = (void *)sparc_isr_vector[n];
    sparc_isr_vector[n] = (air_u32_t)handler;
    return old_handler;
}

void *sparc_install_vector_trap_handler(air_u32_t n, void *handler) {

(void)    sparc_install_raw_trap_handler(n, sparc_isr_handler_entry);

    void *old_handler = (void *)sparc_isr_vector[n];
    sparc_isr_vector[n] = (air_u32_t)handler;
    return old_handler;
}
