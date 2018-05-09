/**
 *  @file 
 *  @brief This file contains the driver for the Gaisler's MIL-STD1553B interface
 *
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT
 *
 *  @author Cl�udio Silva
 *
 *	This file contains the generic driver interface for the Gaisler's MIL-STD1553B
 *  core. These functions do not depend on the device's operating mode. 
 */

#include <rtems.h>
#include <amba.h>
#include <ambapp.h>
#include <stdint.h>

#include <iop_error.h>
#include <iop_support.h>

#include <IOPgr1553b.h>
#include <IOPgr1553bc.h>
#include <IOPgr1553rt.h>
#include <IOPgr1553b_config.h>

#include <pprintf.h>
/* Pointer to amba configuration */
static amba_confarea_type *amba_bus;

static grb_priv *bdevs;
static int number_grb_devices;
static int is_init = 0;

rtems_device_driver grb_initialize(rtems_device_major_number major,
								   rtems_device_minor_number minor,
								   void *arg)
{	
	if (is_init == 0)
	{
	/* Current device */
	grb_priv *bDev;

	/* Amba APB device */
	amba_apb_device ambadev;
	
	/* Get amba bus configuration */
	amba_bus = &amba_conf;
	
	/* Iterator through all devices */
	int i;
	
	/* Iterator through shortcuts */
	int j;
	
	/* get memory for the devices internal structure */
	bdevs = iop_grb_get_priv_mem();
	
	/* Number of gr1553b devices available  */
	number_grb_devices = iop_get_number_grb_cores();
	
	/* Verify if there are any devices */
	if(number_grb_devices < 1){
		
		/* Too few devices */
		return RTEMS_TOO_MANY;
	}
	
	/* zero out device structure  */
	memset(bdevs, 0, number_grb_devices * sizeof(grb_priv));
	
        /* Make sure the GR1553 core is enabled */
        clock_gating_enable(amba_bus, GATE_1553);

	for(i = 0; i < number_grb_devices; i++){
		
		bDev = &bdevs[i];
		
		/* Get AMBA AHB device info from Plug&Play */
		if(amba_find_next_apbslv(amba_bus, VENDOR_GAISLER, GAISLER_GR1553B,&ambadev, i) == 0){
			
			/** @todo RAISE_APPLICATION_ERROR()*/
			
			/* Device not found */
			return RTEMS_NOT_DEFINED;
		}
		
		/* Copy pointer to device's memory mapped registers */
		bDev->regs = (void *)ambadev.start;
		
		/* Copy device's minor*/
		bDev->minor = i;
		
		/* Clear IRQ since it is not used*/
		bDev->irq = 0;
		
		/* Obtain user configuration */
		bDev->user_config = iop_grb_get_user_config(i);
		
		/* Don't continue if the core is not configured */
		if(bDev->user_config == NULL){
			
			/* kill this process */
			iop_raise_error(CONFIGURATION_ERROR);
		}
		
		/* Clear pointers. These fields will be filled later*/
		bDev->cl = NULL;
		bDev->cl_size = 0;
		bDev->mem_start = NULL;
		bDev->buf_mem_start = (milstd_data_buf *) NULL;
		bDev->sync = NULL;
		bDev->last_read = NULL;
		
		/* Stop IRQ generation */
		GR1553B_WRITE_REG(&bDev->regs->imask, 0);
		GR1553B_WRITE_REG(&bDev->regs->irq, 0xffffffff);
		
		/* Stop BC if not already stopped (just in case) */
		GR1553B_WRITE_REG(&bDev->regs->bc_ctrl, 0x15520204);
		
		/* Stop RT rx (just in case) */
		GR1553B_WRITE_REG(&bDev->regs->rt_cfg, 0x15530000);
		
		/* Stop BM logging (just in case) */
		GR1553B_WRITE_REG(&bDev->regs->bm_ctrl, 0);
		
		/* Verify if BM mode is supported  */
		if(GR1553B_READ_REG(&bDev->regs->bm_stat) & GR1553B_BM_STAT_BMSUP){
			bDev->available_modes |= FEAT_BM;
		}
		if(GR1553B_READ_REG(&bDev->regs->bc_stat) & GR1553B_BC_STAT_BCSUP){
			bDev->available_modes |= FEAT_BC;
		}
		if(GR1553B_READ_REG(&bDev->regs->rt_stat) & GR1553B_RT_STAT_RTSUP ){
			bDev->available_modes |= FEAT_RT;
		}
		
		/* Device has not started (yet), but it is ready to do it */
		bDev->started = 0;
		
		/* iterate over all shortcut chains */
		for(j = 0; j<32; j++){
			
			/*  initialize them as empty */
			iop_chain_initialize_empty(&bDev->shortcut[j]);
		}
	}
	
	is_init = 1;
	}
	
	return RTEMS_SUCCESSFUL;
}

rtems_device_driver grb_open(rtems_device_major_number major,
						     rtems_device_minor_number minor,
						     void *arg)
{
	/* return code */
	rtems_status_code status = RTEMS_SUCCESSFUL;
	
	/* Current device */
	grb_priv *bDev;
	
	/* Get device's internal structure */
	bDev = &bdevs[minor];
	
	/* Initialize the device according to the selected operative mode */
	switch (bDev->user_config->operating_mode) {
		
		/* Bus Controller */
		case GR1553B_MODE_BC:
		
			/* init bus controller */
			gr1553bc_device_init(bDev);
			break;
		
		/* Remote Terminal */
		case GR1553B_MODE_RT:
		
			/* init Remote Terminal */
			gr1553rt_device_init(bDev);
			break;
		
		/* Bus Monitor */
		case GR1553B_MODE_BM:
			break;
		
		/* Should not happen*/
		default:
			status = RTEMS_NOT_DEFINED;
			break;
	}
	
	return status;

}
