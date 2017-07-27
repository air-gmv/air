/** 
 * 	@file IOPspw_tasks.c
 * 
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT 
 *
 * 	@ingroup TASKS
 *
 *	@author Cl�udio Silva
 *
 * 	@brief Tasks that write and read from GRspw
 *
 */
 
#include <iop.h>
#include <iop_mms.h>
#include <iop_error.h>
#include <iop_support.h>
#include <spw_support.h>


/**
 *  @brief Task that writes pending write requests to SPW
 *  @param [in] arg: not used
 *
 *  Obtains write requests from the send queue chain and writes them to SPW.
 *  In case of a failed write, if the user requested a reply he is informed that
 *  the write failed and the write request is discarded immediately. Is then up
 *  to the user to take the necessary actions.\n
 *  If the user didn't request a reply then the write will be retried until the
 *  request times out.
 */
rtems_task spw_writer(rtems_task_argument arg){

    iop_debug(" :: IOP - spw-writer start!\n");

    /* get task physical device */
    iop_physical_device_t *pdev = (iop_physical_device_t *)arg;

    /* initialize error chain (packets to be re-sent) */
    iop_chain_control error;
    iop_chain_initialize_empty(&error);

    /* get underlying driver */
    iop_spw_device_t *spw_driver = (iop_spw_device_t *)pdev->driver;

    /* main task loop */
    for(;;){

        /* wait for next partition release point */
        iop_task_sleep(0);

        iop_debug(" :: IOP - spw-writer running!\n");

        /* empty send queue */
        while (!iop_chain_is_empty(&pdev->sendqueue)) {

            iop_wrapper_t *wrapper = obtain_wrapper(&pdev->sendqueue);
				
            /* write to the device */
			int error;
            if ( (error = spw_driver->dev.write((iop_device_driver_t *)spw_driver,
                wrapper)) == RTEMS_SUCCESSFUL){
					

				uint32_t j;
				iop_debug(" :: spw_tasks:writer:   MESSAGE WROTE with head_off: %d, head_size: %d, load_off: %d, load_size %d:   ",
						wrapper->buffer->header_off, wrapper->buffer->header_size, wrapper->buffer->payload_off, wrapper->buffer->payload_size);
				
				iop_debug("\n    HEADER:   ");
				for (j = wrapper->buffer->header_off; j < sizeof(iop_header_t); j++)
					iop_debug("%x ", *((uint8_t *)wrapper->buffer->v_addr+j));
				iop_debug("   HEADER ENDED\n    MESSAGE:   ");	
				uint32_t size = wrapper->buffer->payload_size + wrapper->buffer->payload_off;
				for (j = wrapper->buffer->payload_off; j < size; j++)
					iop_debug("%x ", *((uint8_t *)wrapper->buffer->v_addr+j));
				iop_debug("   ENDED\n");
				
				release_wrapper(wrapper);

            /* error sending packet */
            } else {
				iop_debug("ERROR WRITING : %d", error);
                iop_chain_append(&error, &wrapper->node);
                iop_raise_error(HW_WRITE_ERROR);
            }
        }

        /* re-queue failed transmissions */
        while (!iop_chain_is_empty(&error)) {

            iop_wrapper_t *wrapper = obtain_wrapper(&error);
            iop_chain_append(&pdev->sendqueue, &wrapper->node);
        }
    }
}

/**
 *  @brief Task that polls spw0 for new data packets
 *  @param [in] arg: not used
 *
 *  This tasks polls for new data and places it on a reply structure.
 *  Data is validated against acceptable values. Incoming packets other than 
 *  UDP/IP and ARP are discarded. Ip packets are validated by the UDP/IP stack.
 *  Not valid packets are discarded.
 *  @see uip_validate_ip_packet
 *
 *  Any incoming ARP requests are replied immeditely.\n
 *  Failed reads are reported to FDIR
 *  
 */


rtems_task spw_reader(rtems_task_argument arg){

    iop_debug(" :: IOP - spw-reader start!\n");

    /* get task physical device */
    iop_physical_device_t *pdev = (iop_physical_device_t *)arg;

    /* initialize error chain (packets to be resent) */
    iop_chain_control error;
    iop_chain_initialize_empty(&error);

    /* get underlying driver */
    iop_spw_device_t *driver = (iop_spw_device_t *)pdev->driver;

    /* main task loop */
    for (;;) {

        /* wait for next partition release point */
        iop_task_sleep(0);

        iop_debug(" :: IOP - spw-reader running!\n");

        uint32_t i;
        uint32_t skip;
        uint32_t reads = pdev->reads_per_period[xky_schedule.current_schedule_index];
        for (i = 0; i < reads; ++i){

            /* get an empty reply wrapper */
            iop_wrapper_t *wrapper = obtain_free_wrapper();

            /* sanity check */
            if (wrapper == NULL) {
                iop_raise_error(OUT_OF_MEMORY);
                break;
            }
			
            /* read from the device */
            if (driver->dev.read((iop_device_driver_t *)driver, wrapper) == RTEMS_SUCCESSFUL) {
				uint32_t j;
				iop_debug("MESSAGE RECEIVED");
				uint32_t size = wrapper->buffer->header_size;
				iop_debug("\n HEADER:   ");
				for (j = 0; j < size; j++)
					iop_debug("%x", (uintptr_t)wrapper->buffer->v_addr+j);
				iop_debug("   HEADER ENDED\n MESSAGE:   ");	
				size += wrapper->buffer->payload_size;
				for (j; j < size; j++)
					iop_debug("%x", (uintptr_t)wrapper->buffer->v_addr+j);
				iop_debug("   ENDED\n");
				
				iop_chain_append(&pdev->rcvqueue, &wrapper->node);
				wrapper = NULL;
            }



            /* free wrapper if it wasn't used */
            if (wrapper != NULL) {
                release_wrapper(wrapper);
            }
        }
    }
}


rtems_task spwrtr_reader(rtems_task_argument arg){

    iop_debug(" :: IOP - spwrtr-reader start!\n");
    for (;;) {

        /* wait for next partition release point */
        iop_task_sleep(0);

        iop_debug(" :: IOP - spwrtr-reader running!\n");
	}
}

rtems_task spwrtr_writer(rtems_task_argument arg){

    iop_debug(" :: IOP - spwrtr-writer start!\n");
    for (;;) {

        /* wait for next partition release point */
        iop_task_sleep(0);

        iop_debug(" :: IOP - spwrtr-writer running!\n");
	}
}
