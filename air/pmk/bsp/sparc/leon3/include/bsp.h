/*
 * Copyright (C) 2008-2012  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief AIR LEON 3 & 4 Board Support package
 */

#ifndef __BSP_H__
#define __BSP_H__

/**
 * @defgroup bsp_leon BSP - LEON 3 & 4 Abstractions
 * @brief Board Support Package abstractions for LEON 3 & 4 boards
 * @{
 */

#ifndef ASM

#include <cpu.h>
#include <pmk.h>
#include <clock.h>
#include <irqmp.h>
#include <console.h>
#include <workspace.h>
#include <amba.h>

#ifdef PMK_DEBUG
#include <printk.h>
#endif

extern void leon_idle_loop();


/**
 * @brief Activate clock tick interrupt on core 0
 */
void bsp_clock_start(void);


/**
 * @brief BSP Get current core index
 */
static inline air_u32_t bsp_get_core_id(void) {

    air_u32_t id;
    __asm__ __volatile__("rd     %%asr17,%0\n\t" : "=r" (id) : );
    return ((id >> 28) & 0xFF);
}

/**
* @brief Detect board's silicon revision number
*/
static inline air_u32_t bsp_get_revision_version(void){
#ifdef PMK_DEBUG
    printk("Detecting GR740 Revision... ");
#endif
    /* Read MPSTAT register for Internal controler 1 */
    air_u32_t* mpstat = (air_u32_t*) 0xFF904010 ;
    /* Isolate 26th bit which contains an indicaiton on the silicon revision version */
    air_u32_t revision_version = (*mpstat >> 26) & 1 ;
#ifdef PMK_DEBUG
    printk("%d ", revision_version);
#endif
    return revision_version ;
}

/**
 * @brief BSP Initialization
 * @return 0 if the device was initialize successfully, 1 otherwise
 */
int bsp_core_init(void);

/**
 * @brief BSP boot core
 * @param idx Index of the core to boot
 * @param entry Core execution entry point
 */
#define bsp_boot_core(idx, entry) \
        irqmp_boot_core((idx), (entry))
/**
 * @brief Get the maximum available cores
 * @return Number of cores available
 */
#define bsp_get_number_of_cores() \
        irqmp_get_number_of_cores()

#define bsp_interrupt_core(core, tn)\
        irqmp_interrupt_core((core), (tn))

#define bsp_interrupt_broadcast(tn)\
        irqmp_interrupt_broadcast((tn))

#define bsp_get_timer_irq(minor)\
        LEON_get_clock_interrupt_number()

/**
 * @brief BSP Core idle loop
 */
#define bsp_idle_loop\
        leon_idle_loop\

/**
 * @brief Send a byte to the console
 * @param ch byte to send
 */
#define bsp_console_outbyte(ch) \
        console_outbyte((ch))

/**
 * @brief BSP Core ready
 * @note This function is called when a core finish its initialization code
 */
void bsp_core_ready();

/**
 * @brief BSP restart core
 * @param core Executing core control structure
 */
void bsp_restart_core(pmk_core_ctrl_t *core);

/**
 * @brief BSP shtudown core
 * @param core Executing core control structure
 */
void bsp_shutdown_core(pmk_core_ctrl_t *core);

/**
 * @brief BSP specific segregation
 * @param partition Partition to segregate
 */
void bsp_segregation(pmk_partition_t *partition);


static inline void bsp_core_context_save(pmk_core_ctrl_t core) {

    //core_context_save(core);
}

static inline void bsp_core_context_restore(pmk_core_ctrl_t core) {

    //core_context_restore(core);
}









#endif  /* ASM */

/**
 * @brief Interrupt number used for Inter-Processor communication
 */
#define BSP_IPC_IRQ         0x1D
/**
 * @brief Interrupt number used for Inter-Processor context-switch
 */
#define BSP_IPC_PCS         0x1C



#define ASI_LEON_MMUREGS    0x19    /* ASI code for the LEON MMU Registers  */
#define AC_M_SFSR           0x0300  /* synchronous Fault Status Reg         */
#define AC_M_SFAR           0x0400  /* synchronous Fault Address Reg        */

/** @} */

#endif  /* __BSP_H__ */
