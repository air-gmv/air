/** 
 * @file IOPinit.c
 * 
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT 
 * 
 * @author Cláudio Silva
 *
 * @brief IO Partition initialization procedures
 *
 * Initializes tasks, ports, memory and internal IOP structures based on user
 * configured options.
 * 
 */ 

/**
 * @todo
 *  - Error Handling. Lets assume that there are no configuration errors. 
 *	  We also need to now which capabilies are available to us (e.g. reboot partition)\n
 *	  The following recovery options are to be defined:\n
 *		o Can't Create Ports \n
 *		o Can't extract replies or requests \n
 *		o Can't send or receive queueing messages \n
 *      o Error initializing HW \n
 *		o Errors when creating or using RTEMS objects (tasks, rate monotonics, etc) \n
 *      o Deadline miss.\n
 *      o HW errors that affect HW normal behavior \n
 *		o Incomming packet that has a greater size than the one of the a653 port
 *	- Internal Schedule. Find the best way to implement it, and then implement IT!\n
 *  - Think about the reply system in the 1:N case. Iff only one fails should we
 *    	reply with an error condition?\n
 *  - Isolate more properly the configuration files from the non configuration files. 
 *     Avoid compiling other files when only the configuration has changed\n
 *  - The extract_request and extract_reply need to be protected against preemption.
 *     Not protected by a semaphore but by a disable interrupt?\n
 *  - Add the cache disabled malloc \n
 *  - Reuse unused requests instead of being always extracting and appending 
 */
 
#include <iop.h>
#include <rtems.h>
#include <string.h>
#include <iop_mms.h>
#include <iop_support.h>
#include <iop_error.h>

//#define IOP_MAIN_DEBUG
#ifdef IOP_MAIN_DEBUG
#include <ambapp.h>
#include <amba.h>
#endif

/**
 * @brief Initializes the free wrappers chain queues
 */
static void iop_init_queues(void){

    iop_debug(" :: IOP - initializing queues (%i)\n",
              usr_configuration.wrappers_count);

    /* setup IOP buffers */
    setup_iop_buffers(
            usr_configuration.iop_buffers,
            usr_configuration.iop_buffers_storage,
            usr_configuration.wrappers_count);

    /* append buffers to the wrappers */
    uint32_t i;
    for (i = 0; i < usr_configuration.wrappers_count; ++i) {
        usr_configuration.wrappers[i].buffer = &usr_configuration.iop_buffers[i];
    }

    /* initialize chain of empty wrappers*/
    iop_chain_initialize(
            &usr_configuration.free_wrappers,
            (void *)usr_configuration.wrappers,
            usr_configuration.wrappers_count, sizeof(iop_wrapper_t));
}

/**
 * @brief Initializes physical and logical devices.
 */
static void iop_init_devs() {

    int i, j;

    iop_debug(" :: IOP - initializing logical devices (%i)\n",
              usr_configuration.logical_devices.length);

    /* iterate over all logical devices */
    for (i = 0; i < usr_configuration.logical_devices.length; ++i) {

        /* get logical device */
        iop_logical_device_t *ldev = get_logical_device(i);

        /* initialize logical device chains without any elements */
        iop_chain_initialize_empty(&ldev->sendqueue);
        iop_chain_initialize_empty(&ldev->rcvqueue);

        /* initialize headers based on the corresponding physical device*/
        for (j = 0; j < ldev->routes.length; ++j) {

            /* route pointer */
            iop_logical_route_t *route = get_logical_route(ldev, j);

            /* pre-build header if necessary */
            if (route->device->header_prebuild != NULL) {
                route->device->header_prebuild(route->header);
            }
        }
    }

    iop_debug(" :: IOP - initializing physical devices (%i)\n",
              usr_configuration.physical_devices.length);

	/* initialize the physical device queues */
    for (i = 0; i < usr_configuration.physical_devices.length; ++i) {

        /* get physical device */
        iop_physical_device_t *pdev = get_physical_device(i);

        /* initialize physical device's chains without any elements */
        iop_chain_initialize_empty(&pdev->rcvqueue);
        iop_chain_initialize_empty(&pdev->sendqueue);

        /* initialize headers */
        for (j = 0; j < pdev->routes.length; ++j) {

            /* route pointer */
            iop_physical_route_t *route = get_physical_route(pdev, j);

            /* pre-build header if necessary */
            if (pdev->header_prebuild != NULL) {
                pdev->header_prebuild(route->header);
            }
        }
    }
}

/**
 *	\fn iop_init_drivers(void)
 *  \return rtems_status_code 
 *  \brief Initializes drivers and HW cores
 *  
 */
static rtems_status_code iop_init_drivers(void){

    iop_debug(" :: IOP - device drivers\n");

    int i;
    rtems_status_code rc = RTEMS_SUCCESSFUL;
    for (i = 0; i < usr_configuration.physical_devices.length; ++i) {

        rtems_status_code drc = rc = RTEMS_SUCCESSFUL;

        /* get physical device */
        iop_physical_device_t *pdev = get_physical_device(i);
		
		char *init_func = &(pdev->driver->init);
		iop_debug("  :: IOP - physical devices  %x\n", *(init_func));

        /* initialize device */
        if (pdev->driver->init != NULL) {
            drc = pdev->driver->init(pdev->driver, NULL);
        }

        /* open device */
        if (RTEMS_SUCCESSFUL == drc && pdev->driver->open != NULL) {
            drc = pdev->driver->open(pdev->driver, NULL);
        }

        /* account for this device state */
        rc |= drc;
    }

	return rc;
}

/**
 *	\fn iop_init_ports(void)
 *  \return rtems_status_code:
 *  \brief Initializes Queueing ports used for communication with other partitions
 *  
 *  Creates queueing ports with the paramaters obtained from the configuration.
 *  The order in which the configuration is obatined is relevant to the 
 *  behavior of the IOP. #iop_port_configuration
 */
static rtems_status_code iop_init_ports() {

    int i;
    rtems_status_code rc = RTEMS_SUCCESSFUL;

    iop_debug(" :: IOP - initializing ports (%i)\n",
              usr_configuration.remote_ports.length);

    for (i = 0; i < usr_configuration.remote_ports.length; ++i) {

        /* get port */
        iop_port_t *port = get_remote_port(i);

        /* create port */
        xky_status_code_e p_rc = xky_syscall_create_port(
                port->type, port->name,
                port->configuration, &port->id);

        /* check return code */
        if (p_rc != XKY_NO_ERROR && p_rc != XKY_NO_ACTION) {
            iop_debug("    - error %i creating port %s\n", p_rc, port->name);
            rc = RTEMS_INTERNAL_ERROR;
        }
    }

    return rc;
}

/**
 * @brief IO partition entry point
 * @return RETURN_CODE_TYPE: Status of the operation. 
 *
 * Initializes the IOPartion components
 */
rtems_status_code IOPinit() {

    iop_debug("Initializing IOP\n");

	/* initialize queues */
	iop_init_queues();

	/* initialize mms */
	iop_init_mms();

	/* initialize ports  */
	if (iop_init_ports() != RTEMS_SUCCESSFUL) {
		iop_raise_error(CANT_CREATE_PORT);
	}
	
	/* initialize Devices and respective routes */
	iop_init_devs();

#ifdef IOP_MAIN_DEBUG
	/* Pointer to amba bus structure*/
	amba_confarea_type *amba_bus;
	
	/*Get amba bus configuration*/
	amba_bus = &amba_conf;
	iop_debug("amba_conf->ahbmst: %d\n", amba_bus->ahbmst.devnr);
	iop_debug("amba_conf->ahbslv: %d\n", amba_bus->ahbslv.devnr);
	iop_debug("amba_conf->apbslv: %d\n", amba_bus->apbslv.devnr);
	
	unsigned int conf;
	int k;
	for(k = 0; k < amba_bus->ahbmst.devnr; k++)
    {
        /* get the configuration area */
        conf = amba_get_confword(amba_bus->ahbmst , k , 0);

		iop_debug("ahbmst:%d  AMBA VENDOR: 0x%x   AMBA DEV: 0x%x   conf: 0x%x\n",
					k, amba_vendor(conf), amba_device(conf), conf);
    }
	for(k = 0; k < amba_bus->ahbslv.devnr; k++)
    {
        /* get the configuration area */
        conf = amba_get_confword(amba_bus->ahbslv , k , 0);

		iop_debug("ahbslv:%d  AMBA VENDOR: 0x%x   AMBA DEV: 0x%x   conf: 0x%x\n",
					k, amba_vendor(conf), amba_device(conf), conf);
    }
	for(k = 0; k < amba_bus->apbslv.devnr; k++)
    {
        /* get the configuration area */
        conf = amba_get_confword(amba_bus->apbslv , k , 0);

		iop_debug("apbslv:%d  AMBA VENDOR: 0x%x   AMBA DEV: 0x%x   conf: 0x%x\n",
					k, amba_vendor(conf), amba_device(conf), conf);
    }
#endif

	/* initialize Drivers*/
	if (iop_init_drivers() != RTEMS_SUCCESSFUL){
		iop_raise_error(HW_PROBLEM);
	}
	
	iop_main_loop();

	return RTEMS_SUCCESSFUL;
}
