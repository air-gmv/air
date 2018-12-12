/** 
 * 	@file IOPeth_tasks.c
 * 
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT 
 *
 * 	@ingroup TASKS
 *
 *	@author Claudio Silva
 *
 * 	@brief Tasks that write and read from GRETH
 *
 */

#include <iop.h>
#include <iop_mms.h>
#include <iop_error.h>
#include <iop_support.h>
#include <eth_support.h>

/**
 *  @brief Task that writes pending write requests to ETH0
 *  @param [in] arg: not used
 *
 *  Obtains write requests from the send queue chain and writes them to ETH0.
 *  In case of a failed write, if the user requested a reply he is informed that
 *  the write failed and the write request is discarded immediately. Is then up
 *  to the user to take the necessary actions.\n
 *  If the user didn't request a reply then the write will be retried until the
 *  request times out.
 */
void
eth_writer(iop_physical_device_t * pdev)
{

    /* get task physical device */

    if (pdev == NULL)
    {
        iop_debug(" :: IOP - eth_writer parameter cannot be NULL!\n");
        return;
    }

    /* initialize error chain (packets to be re-sent) */
    iop_chain_control error;

    iop_chain_initialize_empty(&error);

    /* get underlying driver */
    iop_eth_device_t *eth_driver = (iop_eth_device_t *) pdev->driver;

    iop_debug(" :: IOP - eth-writer running!\n");

    /* empty send queue */
    while (!iop_chain_is_empty(&pdev->sendqueue))
    {
        iop_wrapper_t *wrapper = obtain_wrapper(&pdev->sendqueue);

        /* write to the device */
        if (eth_driver->dev.write((iop_device_driver_t *) eth_driver,
                                  wrapper) == RTEMS_SUCCESSFUL)
        {

            release_wrapper(wrapper);
            /* error sending packet */
        }
        else
        {

            iop_chain_append(&error, &wrapper->node);
            iop_raise_error(HW_WRITE_ERROR);
        }
    }

    /* re-queue failed transmissions */
    while (!iop_chain_is_empty(&error))
    {
        iop_wrapper_t *wrapper = obtain_wrapper(&error);

        iop_chain_append(&pdev->sendqueue, &wrapper->node);
    }
}

/**
 *  @brief Task that polls eth0 for new data packets
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

void
eth_reader(iop_physical_device_t * pdev)
{
    /* get task physical device */
//    iop_physical_device_t *pdev = (iop_physical_device_t *)arg;

    if (pdev == NULL)
    {
        iop_debug(" :: IOP - eth_reader parameter cannot be NULL!\n");
        return;
    }

    /* initialize error chain (packets to be resent) */
    iop_chain_control error;

    iop_chain_initialize_empty(&error);

    /* get underlying driver */
    iop_eth_device_t *driver = (iop_eth_device_t *) pdev->driver;

    iop_debug(" :: IOP - eth-reader running!\n");

    uint32_t i;
    uint32_t skip;
    uint32_t reads =
        pdev->reads_per_period[air_schedule.current_schedule_index];
    for (i = 0; i < reads; ++i)
    {

        /* get an empty reply wrapper */
        iop_wrapper_t *wrapper = obtain_free_wrapper();

        /* sanity check */
        if (wrapper == NULL)
        {
            iop_raise_error(OUT_OF_MEMORY);
            break;
        }

        /* read from the device */
        if (driver->dev.read((iop_device_driver_t *) driver, wrapper) == 0)
        {
            switch (eth_get_packet_type(wrapper->buffer))
            {
                /* ARP packet */
            case HTONS(ETH_HDR_ARP_TYPE):
                eth_send_arp_reply(driver, wrapper);
                break;

                /* IPv4 packet */
            case HTONS(ETH_HDR_IP_TYPE):
                
                /* check if it valid UDP packet for us */
                if (eth_validate_packet(driver, wrapper))
                {

                    iop_chain_append(&pdev->rcvqueue, &wrapper->node);
                    wrapper = NULL;
                }
                break;
            }
        }

        /* free wrapper if it wasn't used */
        if (wrapper != NULL)
        {
            release_wrapper(wrapper);
        }
    }
}
