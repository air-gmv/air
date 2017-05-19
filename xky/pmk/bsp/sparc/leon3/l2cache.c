/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2015
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file
 * @author pfnf
 * @brief L2 Cache support definitions and functions
 */

#include <pmk.h>
#include <amba.h>
#include <xky_arch.h>
#include <l2cache.h>

/**
 * @brief L2Cache register map structure
 * @ingroup bsp_leon_l2cache
 */
typedef struct {

    volatile xky_u32_t ctrl;             /**< Control register               */
    volatile xky_u32_t stat;             /**< Status register                */
    volatile xky_u32_t flush_mem;        /**< Flush (memory address)         */
    volatile xky_u32_t flust_index;      /**< Flush (set, index)             */
    volatile xky_u32_t dummy[10];        /**< Reserved                       */
    volatile xky_u32_t err_stat;         /**< Error Status/Control           */
    volatile xky_u32_t err_addr;         /**< Error address                  */
    volatile xky_u32_t tag_check;        /**< TAG-check-bit                  */
    volatile xky_u32_t data_check;       /**< Data-check-bit                 */
    volatile xky_u32_t scruber_ctrl;     /**< Scrub Control/Status           */
    volatile xky_u32_t scruber_delay;    /**< Scrub delay                    */
    volatile xky_u32_t err_injection;    /**< Error injection                */

} l2cache_regmap_t;

/**
 * @brief L2 Cache control internal structure
 * @ingroup bsp_leon_l2cache
 */
typedef struct {

    int found;                          /**< Device availability flag       */
    int minor;                          /**< Device minor number            */
    unsigned int irq;                   /**< Device IRQ                     */
    unsigned int nmtrr;                 /**< Number of MTRR registers       */
    unsigned int nways;                 /**< Number of ways                 */
    unsigned int cache_size;            /**< Cache size (in kB)             */
    unsigned int cachel_size;           /**< Cache line size (bytes)        */
    l2cache_regmap_t *regs;             /**< Device configuration registers */

} l2cache_ctrl_t;

/**
 * @brief l2cache_ctrl internal control structure
 * @ingroup bsp_leon_l2cache
 */
static l2cache_ctrl_t l2cache_ctrl;


void l2cache_set_replacement_policy(l2cache_reppol_e repol) {

    /* check if the found was found */
    if (l2cache_ctrl.found == 0) {
        return;
    }

    /* change the replacement policy */
    l2cache_ctrl.regs->ctrl = (l2cache_ctrl.regs->ctrl & ~(0x07 << 28)) | (repol << 28);
    return;
}

void l2cache_flush(xky_uptr_t address, l2cache_rflush_mode_e mode, int disable) {

    /* check if the found was found */
    if (l2cache_ctrl.found == 0) {
        return;
    }

    l2cache_ctrl.regs->flush_mem =
            (address & ~0x1F) | ((disable & 0x01) << 3) | mode;
    return;
}

void l2cache_set_write_policy(l2cache_writepol_e wp) {

    /* check if the found was found */
    if (l2cache_ctrl.found == 0) {
        return;
    }

    /* get cache current enable state */
    xky_u32_t enable_state = l2cache_ctrl.regs->ctrl & (1 << 31);

    /* flush cache */
    l2cache_flush(0x0, INV_AND_WB_ALL_LINES, 1);

    switch (wp)
    {
        /* Set copy back mode */
        case COPY_BACK:
            l2cache_ctrl.regs->ctrl = (l2cache_ctrl.regs->ctrl & ~(1 << 1));
            break;

        /* Set write-through mode */
        case WRITE_THROUGH:
            l2cache_ctrl.regs->ctrl = (l2cache_ctrl.regs->ctrl & ~(1 << 1)) | (1 << 1);
            break;
    }

    /* return cache to the previous enable state */
    l2cache_ctrl.regs->ctrl |= enable_state;
    return;
}

void l2cache_set_number_of_locked_ways(int locked) {

    /* check if the found was found */
    if (l2cache_ctrl.found == 0) {
        return;
    }

    /* check if the number of ways is valid */
    if (locked > l2cache_ctrl.nways) {
        return;
    }

    /* set number of locked ways */
    l2cache_ctrl.regs->ctrl = (l2cache_ctrl.regs->ctrl & ~(0x07 << 8)) |
                         ((locked & 0x07) << 8);
    return;
}

void l2cache_set_bus_usage_mode(l2cacge_statys_mode_e mode) {

    /* check if the found was found */
    if (l2cache_ctrl.found == 0) {
        return;
    }

    /* set number of locked ways */
    l2cache_ctrl.regs->ctrl = (l2cache_ctrl.regs->ctrl & ~(1 << 3)) | (mode << 3);
    return;
}

void l2cache_hit_rate_mode(l2cacge_statys_mode_e mode) {

    /* check if the found was found */
    if (l2cache_ctrl.found == 0) {
        return;
    }

    /* set number of locked ways */
    l2cache_ctrl.regs->ctrl = (l2cache_ctrl.regs->ctrl & ~(1 << 2)) | (mode << 2);
    return;
}

void l2cache_enable_core(void) {

    /* check if the found was found */
    if (l2cache_ctrl.found == 0) {
        return;
    }

    /* enable core */
    l2cache_flush(0x0, INV_AND_WB_ALL_LINES, 0);
    return;
}

void l2cache_disable_core(void) {

    /* check if the controler was found */
    if (l2cache_ctrl.found == 0) {
        return;
    }

    /* disable core */
    l2cache_flush(0x0, INV_AND_WB_ALL_LINES, 1);
    return;
}

int l2cache_init(void) {

    amba_ahb_dev_t dev;

    /* look for the L2Cache device */
    l2cache_ctrl.found = amba_get_ahb_device(
            &amba_confarea, VENDOR_GAISLER, GAISLER_L2CACHE, &dev);

    if (l2cache_ctrl.found > 0) {

        /* store the device data */
        l2cache_ctrl.minor = dev.ver;
        l2cache_ctrl.irq   = dev.irq;
        l2cache_ctrl.regs  = (l2cache_regmap_t *)dev.start[1];

        /* extract device capabilities */
        l2cache_ctrl.cachel_size = (((l2cache_ctrl.regs->stat & (1 << 24)) >> 24) + 1) * 32;
        l2cache_ctrl.nmtrr = ((l2cache_ctrl.regs->stat & ~0x3F0000) >> 16);
        l2cache_ctrl.cache_size = ((l2cache_ctrl.regs->stat & ~0x1FFC) >> 1);
        l2cache_ctrl.nways = (l2cache_ctrl.regs->stat & 0x02) * 2;
        return 0;
    }

    return 1;
}
