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
#include <bsp.h>
#include <rtems.h>
#include <rtems/rtems_bsdnet.h>

#include <network_interface_add.h>

#include "networkconfig.h"

#include "config_leon3_drvmgr.c"

void system_init(void)
{
printk(">>system_init2()\n");
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GRETH
    /* Initializing Driver Manager if not already performed by BSP */
    system_init2();
    printk("Initializing manager Config\n");
    if (drvmgr_init())
    {
        printk("Driver manager Failed to initialize\n");
        exit(-1);
    }
#endif

    ///* Print Driver manager drivers and their assigned devices
printk(">>system_init(alpha)\n");
    drvmgr_summary();
printk(">>system_init(bravo)\n");
    drvmgr_info_drvs(OPTION_INFO_ALL);
printk(">>system_init(charlie)\n");
    drvmgr_print_devs(0xfffff);
printk(">>system_init(delta)\n");
    drvmgr_print_topo();
    // */

    //#ifdef TURNOFF
    /* Init network */
    printk("Initializing network\n");
    rtems_bsdnet_initialize_network ();
    printk("Initializing network DONE\n\n");
    rtems_bsdnet_show_inet_routes();
    printk("\n");
    rtems_bsdnet_show_if_stats();
    //#endif
}
