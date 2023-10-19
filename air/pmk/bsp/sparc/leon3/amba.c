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
 * @brief AMBA BUS setup and lookup functions
 */

#include <amba.h>

/**
 * @brief AMBA Scan depth
 * @ingroup bsp_leon_amba
 */
#define SCAN_DEPTH                              (64)
/**
 * @brief AMBA AHB configuration size
 * @ingroup bsp_leon_amba
 */
#define AHB_SIZE                                (8)
/**
 * @brief AMBA APB configuration size
 * @ingroup bsp_leon_amba
 */
#define APB_SIZE                                (2)
/**
 * @brief AMBA I/O master area
 * @ingroup bsp_leon_amba
 */
#define MASTER_AREA                             (0xFF000)
/**
 * @brief AMBA I/O slave area
 * @ingroup bsp_leon_amba
 */
#define SLAVE_AREA                              (0x00800)
/**
 * @brief Get vendor Id
 * @param x configuration word
 * @ingroup bsp_leon_amba
 */
#define get_vendor(x)                           (((x) >> 24) & 0x00FF)
/**
 * @brief Get device Id
 * @param x configuration word
 * @ingroup bsp_leon_amba
 */
#define get_device(x)                           (((x) >> 12) & 0x0FFF)
/**
 * @brief Get version
 * @param x configuration word
 * @ingroup bsp_leon_amba
 */
#define get_version(x)                          (((x) >> 5) & 0x1F)
/**
 * @brief Get IRQ number
 * @param x configuration word
 * @ingroup bsp_leon_amba
 */
#define get_irq(x)                              ((x) & 0x1F)
/**
 * @brief Set address bit
 * @param adr register address
 * @param bit bit position
 * @ingroup bsp_leon_amba
 */
#define SET_BIT_REG(adr, bit)               *(volatile unsigned int *)(adr) |= (1<<bit);
/**
 * @brief Clear address bit
 * @param adr register address
 * @param bit bit position
 * @ingroup bsp_leon_amba
 */
#define CLEAR_BIT_REG(adr, bit)             *(volatile unsigned int *)(adr) &= ~(1<<bit);

/**
 * @brief Search an AMBA I/O area for AHP Slaves
 * @param amba_conf Pointer to the AMBA configuration area
 * @param io_area I/O area to be searched
 * @ingroup bsp_leon_amba
 */
static void amba_search_ahb_slaves(
        amba_confarea_t *amba_conf, air_uptr_t io_area) {

    int i;
    unsigned int *dev_config = (unsigned int *)(io_area | MASTER_AREA | SLAVE_AREA);
    for (i = 0; i < SCAN_DEPTH && amba_conf->ahb_slaves.count < AHB_SLAVES; ++i) {

        /* check if the device configuration is valid */
        if (get_vendor(dev_config[0]) != 0) {
            amba_conf->ahb_slaves.addr[amba_conf->ahb_slaves.count] = dev_config;
            ++amba_conf->ahb_slaves.count;
        }

        /* advance to the next device */
        dev_config += AHB_SIZE;
    }
}

/**
 * @brief Search an AMBA I/O area for APB Slaves
 * @param amba_conf Pointer to the AMBA configuration area
 * @param io_area I/O area to be searched
 * @ingroup bsp_leon_amba
 */
static void amba_search_apb_slaves(
        amba_confarea_t *amba_conf, air_uptr_t io_area) {

    int i;
    unsigned int *dev_config = (unsigned int *)(io_area | MASTER_AREA);
    for (i = 0; i < APB_SLAVES/2 && amba_conf->apb_slaves.count < APB_SLAVES; ++i){

        if (get_vendor(dev_config[0]) != 0) {
            amba_conf->apb_slaves.addr[amba_conf->apb_slaves.count] = dev_config;
            amba_conf->apb_slaves.apb_masters[amba_conf->apb_slaves.count] = io_area;
            ++amba_conf->apb_slaves.count;
        }

        /* advance to the next device */
        dev_config += APB_SIZE;
    }

}

/**
 * @brief Search over bridge for more AHB masters
 * @param amba_conf Pointer to the AMBA configuration area
 * @param io_area I/O area to be searched
 * @ingroup 
 */
static void amba_search_bridge(
        amba_confarea_t *amba_conf, air_uptr_t io_area){

    int i;
    unsigned int *dev_config;

    /* plug and play area is ussually the last 4 Kb */
    dev_config = (unsigned int *)(io_area | MASTER_AREA);

    /* look for master devices */
    for(i = 0; (i < SCAN_DEPTH && amba_conf->ahb_masters.count < AHB_MASTERS); i++){

        /* check if the device configuration is valid */
        if (get_vendor(dev_config[0]) != 0) {

            amba_conf->ahb_masters.addr[amba_conf->ahb_masters.count] = dev_config;
            ++amba_conf->ahb_masters.count;
        }

        /* advance to the next device */
        dev_config += AHB_SIZE;
    }

    /* look for AHB slave devices */
    amba_search_ahb_slaves(amba_conf, io_area);
}

/**
 * @brief Search an AMBA I/O area for AHB masters
 * @param amba_conf Pointer to the AMBA configuration area
 * @param io_area I/O area to be searched
 * @ingroup bsp_leon_amba
 */
static void amba_search_ahb_masters(
        amba_confarea_t *amba_conf, air_uptr_t io_area) {

    int i;
    unsigned int *dev_config;

    /* mark devices already scanned */
    int scanned_devices = amba_conf->ahb_slaves.count;

    /* look for master devices */
    dev_config = (unsigned int *)(io_area | MASTER_AREA);
    for (i = 0; i < SCAN_DEPTH; ++i) {

        /* check if the device configuration is valid */
        if (get_vendor(dev_config[0]) != 0) {

            amba_conf->ahb_masters.addr[amba_conf->ahb_masters.count] = dev_config;
            ++amba_conf->ahb_masters.count;
        }

        /* advance to the next device */
        dev_config += AHB_SIZE;
    }

    /* look for AHB slave devices */
    amba_search_ahb_slaves(amba_conf, io_area);

    /* scan bridges in the newly found AHB salves */
    for (i = scanned_devices; i < amba_conf->ahb_slaves.count; ++i) {

        /* get device configuration word */
        unsigned int conf = *amba_conf->ahb_slaves.addr[i];

        /* device is AHB->AHB bus bridge */
if((get_vendor(conf) != 0) == VENDOR_GAISLER &&
            get_device(conf) == GAISLER_AHB2AHB) {

            /* get bridge IO area */
            io_area = *(amba_conf->ahb_slaves.addr[i] + 2);
            if (get_version(conf) != 0 && io_area != (air_uptr_t)NULL){
                amba_search_bridge(amba_conf, io_area);
            }
            continue;
        }

        /* found an AHB->APB bus bridge */
if((get_vendor(conf) != 0) == VENDOR_GAISLER &&
            get_device(conf) == GAISLER_APBMST) {

            /* get bridge IO area */
            io_area = *(amba_conf->ahb_slaves.addr[i] + 4);
            io_area = ((io_area & 0xFFF00000) & ((io_area & 0xFFF0) << 16));

            /* search for APB devices */
            amba_search_apb_slaves(amba_conf, io_area);
            continue;
        }
    }
}

/**
 * @brief Get an AHB device from a given lookup table (masters or slaves)
 * @param amba_conf Pointer to the AMBA configuration area
 * @param table AHB device lookup table
 * @param index Index of the device (-1 for the first entry)
 * @param dev Pointer to hold the device information
 */
static inline void get_ahb_device(
        amba_confarea_t *amba_conf,amba_ahb_table_t *table,
        int index, amba_ahb_dev_t *dev) {

    /* get device addresses */
    int i;
    unsigned int addr;
    for (i = 0; i < 4; ++i) {

        addr = *(table->addr[index] + 4 + i);
        addr = (addr & 0xFFF00000) & ((addr & 0xFFF0) << 16);

        if ((addr & 0xF) == 0x3) {
            addr = ((unsigned int)(amba_conf->io_area) | ((addr) >> 12));
        }

        dev->start[i] = addr;
    }

    /* get device version and IRQ number */
    air_u32_t conf = *table->addr[index];
    dev->ver = get_version(conf);
    dev->irq = get_irq(conf);
}

void amba_setup(amba_confarea_t *amba_conf, air_uptr_t io_area) {

    amba_conf->io_area = io_area;

    /* initialize the total number of devices */
    amba_conf->apb_slaves.count = 0;
    amba_conf->ahb_slaves.count = 0;
    amba_conf->ahb_masters.count = 0;

    /* search I/O area (search is recursive, but bounded) */
    amba_search_ahb_masters(amba_conf, io_area);
}

int amba_get_ahb_master(
        amba_confarea_t *amba_conf, int vendor, int device, int index,
        amba_ahb_dev_t *dev) {

    /* look for the specified device */
    int i, count = 0;
    for (i = 0; i < amba_conf->ahb_masters.count; ++i) {

        /* get configuration word */
        air_u32_t conf = *amba_conf->ahb_masters.addr[i];
        if (get_vendor(conf) == vendor && get_device(conf) == device) {

            /* check if the device index we want */
            if (index == count || index == -1) {
                get_ahb_device(amba_conf, &amba_conf->ahb_masters, i, dev);
                return 1;
            }

            ++count;
        }
    }

    /* device not found */
    return 0;
}

int amba_get_ahb_slave(
        amba_confarea_t *amba_conf, int vendor, int device, int index,
        amba_ahb_dev_t *dev) {

    /* look for the specified device */
    int i, count = 0;
    for (i = 0; i < amba_conf->ahb_slaves.count; ++i) {

        /* get configuration word */
        air_u32_t conf = *amba_conf->ahb_slaves.addr[i];
        if (get_vendor(conf) == vendor && get_device(conf) == device) {

            /* check if the device index we want */
            if (index == count || index == -1) {
                get_ahb_device(amba_conf, &amba_conf->ahb_slaves, i, dev);
                return 1;
            }

            ++count;
        }
    }

    /* device not found */
    return 0;
}

int amba_get_ahb_device(
        amba_confarea_t *amba_conf, int vendor, int device, amba_ahb_dev_t *dev) {

    /* search in masters */
    if (amba_get_ahb_master(amba_conf, vendor, device, -1, dev) == 1) {
        return 1;
    }

    /* search in slaves */
    if (amba_get_ahb_slave(amba_conf, vendor, device, -1, dev) == 1) {
        return 1;
    }

    /* device not found */
    return 0;
}

int amba_get_apb_slave(
        amba_confarea_t *amba_conf, int vendor, int device, int index,
        amba_apb_dev_t *dev) {

    /* look for the specified device */
    int i, count = 0;
    unsigned int addr;
    for (i = 0; i < amba_conf->apb_slaves.count; ++i) {

        /* get configuration word */
        air_u32_t conf = *amba_conf->apb_slaves.addr[i];
        if (get_vendor(conf) == vendor && get_device(conf) == device) {

            /* check if the device index we want */
            if (index == count || index == -1) {

                /* get the device start address */
                addr = *(amba_conf->apb_slaves.addr[i] + 1);
                addr = amba_conf->apb_slaves.apb_masters[i] | \
                        (((addr & 0xFFF00000) >> 12) & ((addr & 0xFFF0) << 4));
                dev->start = addr;

                /* get the device interrupt number */
                dev->irq = get_irq(conf);
                return 1;
            }

            ++count;
        }
    }

    /* device not found */
    return 0;
}

int amba_get_number_apbslv_devices (
        amba_confarea_t * amba_conf, int vendor, int device){

    /* look for the specified device */
    int i, count = 0;
    for (i = 0; i < amba_conf->apb_slaves.count; ++i){

        /* get configuration word */
        air_u32_t conf = *amba_conf->apb_slaves.addr[i];
if(get_vendor(conf) == vendor && get_device(conf) == device)
{
            ++count;
}


    }
    return count;
}

int clock_gating_enable(amba_confarea_t* clk_amba_bus, clock_gating_device core_to_enable)
{
    /* Amba APB device */
    amba_apb_dev_t ambadev;

    /* Get AMBA AHB device info from Plug&Play */
if(amba_get_apb_slave(clk_amba_bus, VENDOR_GAISLER, GAISLER_CLKGATE, 0, &ambadev) == 0)
{
        return 0;
}



    /* From LEON4 UM:
     * To enable the clock for a core, the following procedure should be applied
     * 1. Write a 1 to the corresponding bit in the unlock register
     * 2. Write a 1 to the corresponding bit in the core reset register
     * 3. Write a 1 to the corresponding bit in the clock enable register
     * 4. Write a 0 to the corresponding bit in the core reset register
     * 5. Write a 0 to the corresponding bit in the unlock register
     */

    /* Copy pointer to device's memory mapped registers */
    clkgate_regs *gate_regs = (void *)ambadev.start;

    if(!(gate_regs->clock_enable & (1<<core_to_enable)))
    {
        /* 1. Unlock gate to allow enabling it */
        SET_BIT_REG(&gate_regs->unlock, core_to_enable);

        /* 2. Reset gate */
        SET_BIT_REG(&gate_regs->core_reset, core_to_enable);

        /* 3. Enable gate */
        SET_BIT_REG(&gate_regs->clock_enable, core_to_enable);

        /* 4. Clear gate reset*/
        CLEAR_BIT_REG(&gate_regs->core_reset, core_to_enable);

        /* 5. Lock gate */
        CLEAR_BIT_REG(&gate_regs->unlock, core_to_enable);
    }
    return 1;
}


int clock_gating_disable(amba_confarea_t* clk_amba_bus, clock_gating_device core_to_disable)
{
    /* Amba APB device */
    amba_apb_dev_t ambadev;

    /* Get AMBA AHB device info from Plug&Play */
if(amba_get_apb_slave(clk_amba_bus, VENDOR_GAISLER, GAISLER_CLKGATE, 0, &ambadev) == 0)
{
        return 0;
}



    /* Copy pointer to device's memory mapped registers */
    clkgate_regs *gate_regs = (void *)ambadev.start;

    /* From LEON4 UM:
     * 1. Write a 1 to the corresponding bit in the unlock register
     * 2. Write a 0 to the corresponding bit in the clock enable register
     * 3. Write a 0 to the corresponding bit in the unlock register
     */
    SET_BIT_REG(&gate_regs->unlock, core_to_disable);
    CLEAR_BIT_REG(&gate_regs->clock_enable, core_to_disable);
    CLEAR_BIT_REG(&gate_regs->unlock, core_to_disable);

    return 1;
}
