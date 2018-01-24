/* Configuration file for LEON2, GRLIB-LEON2 and LEON3 systems 
 * 
 * Defines driver resources in separate files,
 *  - LEON3         - leon3_drv_config.c
 *  - LEON2         - leon2_drv_config.c
 *  - LEON2-GRLIB   - leon2_grlib_drv_config.c
 *
 * Initializes,
 *   - Driver manager
 *   - Networking if ENABLE_NETWORK is set
 * 
 */

/* Define for GRLIB LEON2 systems, when a AMBA PnP bus is available */
/*#define LEON2_GRLIB*/

/* Configure Network if enabled */

#include "config_leon3_drvmgr.c"

void system_init(void)
{

//#ifdef RTEMS_DRVMGR_STARTUPxcxc
	/* Initializing Driver Manager if not alread performed by BSP */
	system_init2();
	printk("Initializing manager Config\n");
	if ( drvmgr_init() ) {
		printk("Driver manager Failed to initialize\n");
		exit(-1);
	}
//#endif	
	
//if ( canmux_register() )
 printk("Driver status\n");

	///* Print Driver manager drivers and their assigned devices
	// drvmgr_summary();
	// drvmgr_info_drvs(OPTION_INFO_ALL);
	       drvmgr_print_devs(0xfffff);
    //  drvmgr_print_topo();
	// */
}
