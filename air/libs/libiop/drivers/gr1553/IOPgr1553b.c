/*
 * Copyright (C) 2011  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 *  @file
 *  @brief This file contains the driver for the Gaisler's MIL-STD1553B interface
 *
 *  @author Cl�udio Silva
 *
 *	This file contains the generic driver interface for the Gaisler's MIL-STD1553B
 *  core. These functions do not depend on the device's operating mode.
 */

#include <iop_error.h>
#include <iop_support.h>
#include <bsp.h>
#include <IOPgr1553b.h>
#include <IOPgr1553bc.h>
#include <IOPgr1553rt.h>

uint32_t gr1553b_initialize(iop_device_driver_t *iop_dev, void *arg)
{

    /* Get driver priv struct */
    iop_1553_device_t *device = (iop_1553_device_t *)iop_dev;
    grb_priv *bdev = (grb_priv *)(device->dev.driver);

    /* Amba APB device */
    amba_apb_dev_t ambadev;

    /* Iterator through shortcuts */
    int j;

    amba_confarea_t *ptrarea = (amba_confarea_t *)air_syscall_get_ambaconf();

    /* Make sure the GR1553 core is enabled */
    clock_gating_enable(ptrarea, GATE_1553);

    /* Get AMBA AHB device info from Plug&Play */
    if (amba_get_apb_slave(ptrarea, VENDOR_GAISLER, GAISLER_GR1553B, 0, &ambadev) == 0)
    {

        /** @todo RAISE_APPLICATION_ERROR()*/
        /* Device not found */
        return AIR_DEVICE_NOT_FOUND;
    }

    /* Copy pointer to device's memory mapped registers */
    bdev->regs = (void *)ambadev.start;

    /* Clear IRQ since it is not used*/
    bdev->irq = 0;

    /* Don't continue if the core is not configured */
    if (bdev->user_config == NULL)
    {

        /* kill this process */
        iop_raise_error(CONFIGURATION_ERROR);
        return AIR_INVALID_CONFIG;
    }

    /* Clear pointers. These fields will be filled later*/
    bdev->buf_mem_start = (milstd_data_buf *)NULL;
    bdev->sync = NULL;
    bdev->last_read = NULL;

    /* Stop IRQ generation */
    GR1553B_WRITE_REG(&bdev->regs->imask, 0);
    GR1553B_WRITE_REG(&bdev->regs->irq, 0xffffffff);

    /* Stop BC if not already stopped (just in case) */
    GR1553B_WRITE_REG(&bdev->regs->bc_ctrl, 0x15520204);

    /* Stop RT rx (just in case) */
    GR1553B_WRITE_REG(&bdev->regs->rt_cfg, 0x15530000);

    /* Stop BM logging (just in case) */
    GR1553B_WRITE_REG(&bdev->regs->bm_ctrl, 0);

    /* Verify if BM mode is supported  */
    if ((GR1553B_READ_REG(&bdev->regs->bm_stat) & GR1553B_BM_STAT_BMSUP) != 0)
    {
        bdev->available_modes |= FEAT_BM;
    }
    if ((GR1553B_READ_REG(&bdev->regs->bc_stat) & GR1553B_BC_STAT_BCSUP) != 0)
    {
        bdev->available_modes |= FEAT_BC;
    }
    if ((GR1553B_READ_REG(&bdev->regs->rt_stat) & GR1553B_RT_STAT_RTSUP) != 0)
    {
        bdev->available_modes |= FEAT_RT;
    }

    /* Device has not started (yet), but it is ready to do it */
    bdev->started = 0;

    /* iterate over all shortcut chains */
    for (j = 0; j < 32; j++)
    {

        /*  initialize them as empty */
        iop_chain_initialize_empty(&bdev->shortcut[j]);
    }

    return AIR_SUCCESSFUL;
}

uint32_t gr1553b_open(iop_device_driver_t *iop_dev, void *arg)
{
    /* Get driver priv struct */
    iop_1553_device_t *device = (iop_1553_device_t *)iop_dev;
    grb_priv *bdev = (grb_priv *)(device->dev.driver);

    /* return code */
    air_status_code_e status = AIR_SUCCESSFUL;

    /* Initialize the device according to the selected operative mode */
    switch (bdev->user_config->operating_mode)
    {

    /* Bus Controller */
    case GR1553B_MODE_BC:

        /* init bus controller */
        gr1553bc_device_init(bdev);
        break;

    /* Remote Terminal */
    case GR1553B_MODE_RT:

        /* init Remote Terminal */
        gr1553rt_device_init(bdev);
        break;

    /* Bus Monitor */
    case GR1553B_MODE_BM:
        break;

    /* Should not happen*/
    default:
        status = AIR_INVALID_MODE;
        break;
    }

    return status;
}

uint32_t gr1553b_close(iop_device_driver_t *iop_dev, void *arg)
{
    /* Get driver priv struct */
    iop_1553_device_t *device = (iop_1553_device_t *)iop_dev;
    grb_priv *bdev = (grb_priv *)(device->dev.driver);

    /* return code */
    air_status_code_e status = AIR_SUCCESSFUL;

    /* Initialize the device according to the selected operative mode */
    switch (bdev->user_config->operating_mode)
    {

    /* Bus Controller */
    case GR1553B_MODE_BC:

        /* close Bus Controller */
        gr1553bc_close(bdev);
        break;

    /* Remote Terminal */
    case GR1553B_MODE_RT:

        /* TODO close Remote Terminal */
        //	gr1553rt_device_close(bDev);
        break;

    /* Bus Monitor */
    case GR1553B_MODE_BM:
        break;

    /* Should not happen*/
    default:
        status = AIR_INVALID_MODE;
        break;
    }

    return status;
}
