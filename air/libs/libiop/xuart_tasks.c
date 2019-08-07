/**
 *  @file
 *
 *  COPYRIGHT (c) 2019
 *  GMV-SKYSOFT
 *
 *  @ingroup TASKS
 *
 *  @author trcpse
 *
 *  @brief Tasks that write and read from XUART
 *
 */

#include <iop.h>
#include <iop_mms.h>
#include <iop_error.h>
#include <iop_support.h>
#include <xuart_support.h>


/**
 *  @brief Task that writes pending write requests to UART
 *  @param [in] arg
 *
 *  Obtains write requests from the send queue chain and writes them to UART.
 *  In case of a failed write, if the user requested a reply he is informed that
 *  the write failed and the write request is discarded immediately. Is then up
 *  to the user to take the necessary actions.\n
 *  If the user didn't request a reply then the write will be retried until the
 *  request times out.
 */
void xuart_writer(air_uptr_t arg){

    iop_debug("\n :: IOP - UART-writer running!\n");

    /* get task physical device */
    iop_physical_device_t *pdev = (iop_physical_device_t *)arg;

    /* initialize error chain (packets to be re-sent) */
    iop_chain_control error;
    iop_chain_initialize_empty(&error);

    /* get underlying driver */
    iop_uart_device_t *uart_driver = (iop_uart_device_t *)pdev->driver;

    /* empty send queue */
    while (!iop_chain_is_empty(&pdev->sendqueue)) {

        iop_wrapper_t *wrapper = obtain_wrapper(&pdev->sendqueue);

        /* write to the device */
        if (uart_driver->dev.write((iop_device_driver_t *)uart_driver,
            wrapper) == AIR_SUCCESSFUL){
            release_wrapper(wrapper);

        } else {
            iop_debug(" :: UART ERROR writing");
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
void xuart_reader(air_uptr_t arg){

    iop_debug("\n :: IOP - UART-reader running!\n");

    /* get task physical device */
    iop_physical_device_t *pdev = (iop_physical_device_t *)arg;

    /* initialize error chain (packets to be resent) */
    iop_chain_control error;
    iop_chain_initialize_empty(&error);

    /* get underlying driver */
    iop_uart_device_t *driver = (iop_uart_device_t *)pdev->driver;

    air_u32_t i;
    air_u32_t reads = pdev->reads_per_period[air_schedule.current_schedule_index];
    for (i = 0; i < reads; ++i){

        /* get an empty reply wrapper */
        iop_wrapper_t *wrapper = obtain_free_wrapper();

        /* sanity check */
        if (wrapper == NULL) {
            iop_raise_error(OUT_OF_MEMORY);
            break;
        }

        /* read from the device */
        if (driver->dev.read((iop_device_driver_t *)driver, wrapper) == AIR_SUCCESSFUL) {
            iop_debug("UART reader: Message received");
            iop_chain_append(&pdev->rcvqueue, &wrapper->node);
            wrapper = NULL;
        }

        /* free wrapper if it wasn't used */
        if (wrapper != NULL) {
            release_wrapper(wrapper);
        }
    }
}
