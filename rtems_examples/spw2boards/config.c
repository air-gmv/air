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
#include <bsp/network_interface_add.h>
#include "networkconfig.h"

#include "config_leon3_drvmgr.c"
//#include "config_leon4_n2x.c"

void system_init(void)
{


	/* Initializing Driver Manager if not alread performed by BSP */
	system_init2();
	printf("Initializing manager Config\n");
	if ( drvmgr_init() ) {
		printf("Driver manager Failed to initialize\n");
		exit(-1);
	}


	///* Print Driver manager drivers and their assigned devices
//	 drvmgr_summary();
//	 drvmgr_info_drvs(OPTION_INFO_ALL);
	  //     drvmgr_print_devs(0xfffff);
	// */
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GRETH

//#ifdef TURNOFF
	/* Init network */
	printf("Initializing network\n");
	rtems_bsdnet_initialize_network ();
	printf("Initializing network DONE\n\n");
	rtems_bsdnet_show_inet_routes();
	printf("\n");
	rtems_bsdnet_show_if_stats();
#endif
}
