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

rtems_task pre_dispatcher(rtems_task_argument arg);
rtems_task pre_router(rtems_task_argument arg);
rtems_task pos_dispatcher(rtems_task_argument arg);
rtems_task pos_router(rtems_task_argument arg);

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
<<<<<<< HEAD
	
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
=======
>>>>>>> c92b2bfd32... Some bugs corrected in the configurator. iop_debug's added for debugging.

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
		
		iop_debug("   :: drc = %d\n", drc);

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
<<<<<<< HEAD
=======
 *  \return RTEMS_SUCCESSFUL if the operation was completed correctly
 *  or the status_code returned by rtems_task_create or rtems_task_start
 *
 *  \brief Initializes IOP tasks
 *  
 */
static rtems_status_code iop_init_worker_tasks(void){

    iop_debug(" :: creating & launching worker tasks\n");

    int i;
    rtems_name task_name;
    rtems_status_code rc;
    rtems_id pred_task_id, prer_task_id, posd_task_id, posr_task_id;

    /* create pre-dispatcher task */
    task_name = rtems_build_name( 'P', 'R', 'E', 'D');
    if ((rc = rtems_task_create(task_name, 10, 4096, RTEMS_DEFAULT_MODES,
        RTEMS_DEFAULT_ATTRIBUTES, &pred_task_id)) != RTEMS_SUCCESSFUL) {

        return rc;
    }

    /* create pre-router task */
    task_name = rtems_build_name( 'P', 'R', 'E', 'R');
    if ((rc = rtems_task_create(task_name, 11, 4096, RTEMS_DEFAULT_MODES,
        RTEMS_DEFAULT_ATTRIBUTES, &prer_task_id)) != RTEMS_SUCCESSFUL) {

        return rc;
    }

    /* create device writer and reader tasks */
    for (i = 0; i < usr_configuration.physical_devices.length; ++i) {

        /* get device pointer */
        iop_physical_device_t *dev = get_physical_device(i);

        /* create device writer task */
        task_name = rtems_build_name( 'W', 'R', 'T', (char)i);
        if ((rc = rtems_task_create(task_name, 12, 4096, RTEMS_DEFAULT_MODES,
            RTEMS_DEFAULT_ATTRIBUTES, &dev->writer_id)) != RTEMS_SUCCESSFUL) {

            return rc;
        }

        /* create device reader task */
        task_name = rtems_build_name( 'R', 'D', ' ', (char)i);
        if ((rc = rtems_task_create(task_name, 20, 4096, RTEMS_DEFAULT_MODES,
            RTEMS_DEFAULT_ATTRIBUTES, &dev->reader_id)) != RTEMS_SUCCESSFUL) {

            return rc;
        }
    }

    /* create pos-dispatcher task */
    task_name = rtems_build_name( 'P', 'O', 'S', 'D');
    if (usr_configuration.request_ports.length > 0 &&
        (rc = rtems_task_create(task_name, 40, 4096, RTEMS_DEFAULT_MODES,
        RTEMS_DEFAULT_ATTRIBUTES, &posd_task_id)) != RTEMS_SUCCESSFUL) {
        return rc;
    }

    /* create pos-router task */
    task_name = rtems_build_name( 'P', 'O', 'S', 'R');
    if ((rc = rtems_task_create(task_name, 41, 4096, RTEMS_DEFAULT_MODES,
        RTEMS_DEFAULT_ATTRIBUTES, &posr_task_id)) != RTEMS_SUCCESSFUL) {

        return rc;
    }


    /* start pre-dispatcher task */
    if ((rc = rtems_task_start(pred_task_id, pre_dispatcher,
        (rtems_task_argument)NULL)) != RTEMS_SUCCESSFUL) {
        return rc;
    }

    /* start pre-router task */
    if ((rc = rtems_task_start(prer_task_id, pre_router,
        (rtems_task_argument)NULL)) != RTEMS_SUCCESSFUL) {
        return rc;
    }

    /* start device tasks */
    for (i = 0; i < usr_configuration.physical_devices.length; ++i) {

        /* get device pointer */
        iop_physical_device_t *dev = get_physical_device(i);
		
        /* start device reader task */
        if ((rc = rtems_task_start(dev->reader_id, dev->reader_task,
            (rtems_task_argument)dev)) != RTEMS_SUCCESSFUL) {
            return rc;
        }
		
        /* start device writer task */
        if ((rc = rtems_task_start(dev->writer_id, dev->writer_task,
            (rtems_task_argument)dev)) != RTEMS_SUCCESSFUL) {
            return rc;
        }
    }

    /* start pos-dispatcher task */
    if (usr_configuration.request_ports.length > 0 &&
        (rc = rtems_task_start(posd_task_id, pos_dispatcher,
        (rtems_task_argument)NULL)) != RTEMS_SUCCESSFUL) {
        return rc;
    }


    /* start pos-router task */
    if ((rc = rtems_task_start(posr_task_id, pos_router,
        (rtems_task_argument)NULL)) != RTEMS_SUCCESSFUL) {
        return rc;
    }

    return RTEMS_SUCCESSFUL;
}

/**
>>>>>>> 0a924f0799a09d9f4e65b65a2d2cda1647614e32
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
