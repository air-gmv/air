/*
 * Copyright (C) 2008-2015  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief IRQMP device initialization and handling functions
 */

#include <pmk.h>
#include <cpu.h>
#include <bsp.h>
#include <amba.h>
#include <irqmp.h>
#include <printk.h>
#include <multicore.h>
#include <configurations.h>

/**
 * @brief IRQMP Maximum controllable cores
 * @ingroup bsp_leon_irqmp
 */
#define IRQMP_MAX_CORES                                     (16)

/**
 * @brief IRQMP Control registers map
 * @ingroup bsp_leon_irqmp
 */
typedef struct {

    volatile air_u32_t ilevel;           /**< Interrupt Level                */
    volatile air_u32_t ipend;            /**< Interrupt Pending              */
    volatile air_u32_t iforce;           /**< Force Interrupt                */
    volatile air_u32_t iclear;           /**< Interrupt Clear                */
    volatile air_u32_t mpstat;           /**< Multiprocessor Status          */
    volatile air_u32_t bcast;            /**< Broadcast interrupt            */
    volatile air_u32_t dummy00;        /**< Reserved                       */
    volatile air_u32_t wtchdog_ctrl;     /**< Watch-dog Control              */
    volatile air_u32_t ampctrl;          /**< AMP control                    */
    volatile air_u32_t icsel[2];         /**< IRQ Controller Select          */
    volatile air_u32_t dummy01;          /**< Reserved                       */
    volatile air_u32_t dummy02;          /**< Reserved                       */
    volatile air_u32_t dummy03;          /**< Reserved                       */
    volatile air_u32_t dummy04;          /**< Reserved                       */
    volatile air_u32_t dummy05;          /**< Reserved                       */
    volatile air_u32_t mask[16];         /**< Interrupt Mask per Core        */
    volatile air_u32_t force[16];        /**< Force Mask per Core            */
    volatile air_u32_t intid[16];        /**< Extended Interrupt per core    */
    volatile air_u32_t timestamp[8];     /**< Interrupt Time-stamp Registers */
    volatile air_u32_t dummy06[56];      /**< Reserved                       */
    volatile air_u32_t start_addr[16];   /**< Reset Start Address            */
    volatile air_u32_t boot;             /**< Processor Boot Register        */
    volatile air_u32_t dummy07[0x36F];   /**< Alignment to 4Kb boundary      */

} irqmp_regmap_t;

/**
 * @brief IRQMP Internal control structure
 * @ingroup bsp_leon_irqmp
 */
typedef struct {

    /** Device found                                                        */
    air_u32_t found;
    /** Number of CPUs (Cores)                                              */
    air_u32_t ncpus;
    /** IRQASMP Control registers                                           */
    irqmp_regmap_t *regs[IRQMP_MAX_CORES];

} irqmp_ctrl_t;


/**
 * @brief IRQ level register
 * * @ingroup bsp_leon_irqmp
 */
volatile air_u32_t *IRQ_pedding_register;
/**
 * @brief IRQ level register
 * * @ingroup bsp_leon_irqmp
 */
volatile air_u32_t *IRQ_level_register;
/**
 * @brief IRQ mask register pointer
 * * @ingroup bsp_leon_irqmp
 */
volatile air_u32_t *IRQ_mask_register;
/**
 * @brief IRQ force register pointer
 * * @ingroup bsp_leon_irqmp
 */
volatile air_u32_t *IRQ_force_register;
/**
 * @brief IRQ mask protected interrupts
 * * @ingroup bsp_leon_irqmp
 */
air_u32_t IRQ_mask_protected_interrupts = 0;
/**
 * @brief IRQ force protected interrupts
 * * @ingroup bsp_leon_irqmp
 */
air_u32_t IRQ_force_protected_interrupts = ~0;

/**
 * @brief IRQASMP control and registers
 * * @ingroup bsp_leon_irqmp
 */
static irqmp_ctrl_t irqasmp;

/**
 * @brief Convert a trap to an interrupt number
 * @param tn Trap number
 */
#define trap_to_irq_number(tn)             ((tn) - 0x10)

air_u32_t pmk_force_mask = ~0;
air_u32_t pmk_irq_mask[IRQMP_MAX_CORES] = { 0 };

int irqmp_init(void) {

    amba_apb_dev_t irqmp_device;

    /* device was found */
    if (amba_get_apb_slave(
            &amba_confarea, VENDOR_GAISLER, GAISLER_IRQMP, -1, &irqmp_device)) {
        
        /* map the device registers */
        irqasmp.regs[0] = (irqmp_regmap_t *)irqmp_device.start;

        /* map the Control registers to the easy pointers for assembly */
        IRQ_mask_register = irqasmp.regs[0]->mask;
        IRQ_force_register = irqasmp.regs[0]->force;
        IRQ_pedding_register = &irqasmp.regs[0]->ipend;

        /* link the ASMP extensions if they are available */
        int i;
        irqasmp.ncpus = ((irqasmp.regs[0]->ampctrl >> 28) & 0xF) + 1;
        if (irqasmp.ncpus > 1) {
            for (i = 1; i < irqasmp.ncpus; ++i) {
                irqasmp.regs[i] = ((irqmp_regmap_t *)irqmp_device.start) + i;
            }
        }
        
        irqasmp.found = 1;
        return 0;
    }

    /* device not found */
    irqasmp.found = 0;
    return 1;
}

air_u32_t irqmp_get_number_of_cores(){

    return irqasmp.ncpus;
}

void irqmp_set_interrupt_mask(air_u32_t core_id, air_u32_t irq_mask) {

    IRQ_mask_register[core_id] = irq_mask;
    return;
}

void irqmp_enable_interrupt(air_u32_t core_id, air_u32_t tn) {

    tn = trap_to_irq_number(tn);

    /* check if the core id is valid, and if the interrupt is valid */
    if (core_id < irqasmp.ncpus && tn > 0 && tn < 32){

        pmk_irq_mask[core_id] |= 1 << tn;
        irqasmp.regs[0]->mask[core_id] |= 1 << tn;
        IRQ_mask_protected_interrupts |= (1 << tn);
        IRQ_force_protected_interrupts &= ~(1 << tn);
    }

    return;
}

void irqmp_disable_interrupt(air_u32_t core_id, air_u32_t tn) {

    tn = trap_to_irq_number(tn);

    /* check if the core id is valid, and if the interrupt is valid */
    if (core_id < irqasmp.ncpus && tn > 0 && tn < 32){

        pmk_irq_mask[core_id] &= ~(1 << tn);
        irqasmp.regs[0]->mask[core_id] &= ~(1 << tn);
        IRQ_mask_protected_interrupts &= ~(1 << tn);
        IRQ_force_protected_interrupts |= (1 << tn);
    }

    return;
}

void irqmp_interrupt_set_priority(air_u32_t tn, air_u32_t p) {

    tn = trap_to_irq_number(tn);

    switch (p) {

        /* low priority */
        case 0:
            irqasmp.regs[0]->ilevel &= ~(1 << tn);
            break;

        /* high priority */
        case 1:
            irqasmp.regs[0]->ilevel |= (1 << tn);
            break;

        /* do nothing */
        default:
            break;

    }

    return;
}

void irqmp_interrupt_core(air_u32_t core_id, air_u32_t tn) {

    tn = trap_to_irq_number(tn);

    /* check if core id is valid */
    if (core_id >= irqasmp.ncpus) return;

    /* force interrupt on core id */
    irqasmp.regs[0]->force[core_id] |= 1 << tn;
    return;
}

void irqmp_interrupt_broadcast(air_u32_t tn) {

    tn = trap_to_irq_number(tn);

    /* check if the interrupt number is valid */
    if (tn >= 0 && tn < 15) {

        /* interrupt all cores except the current one */
        air_u32_t i;
        air_u32_t core_id = bsp_get_core_id();
        for (i = 0; i < irqasmp.ncpus; ++i) {
            if (core_id != i) {
                irqasmp.regs[0]->force[i] |= (1 << tn);
            }
        }
    }
    return;
}

void irqmp_boot_core(air_u32_t core_id, void *entry_point) {

    /* check if core id is valid */
    if (core_id >= irqasmp.ncpus) return;

    /* boot core */
    irqasmp.regs[0]->start_addr[core_id] = (air_u32_t)entry_point;
    irqasmp.regs[0]->boot |= ((1 << core_id) & 0xF);
    return;
}

/*
 * Sets the multi processor status
 * Used to start secondary processor_state_register
 */ 
void irqmp_interrupt_set_mprocessor_status (air_u32_t core_id) {
    irqasmp.regs[0]->mpstat = 1U << core_id;
}
