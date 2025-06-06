/*
 * Copyright (C) 2011  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/AIR/raw/master/air/LICENSE
 */
/** 
 * 	@file IOPeth_tasks.c
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
 *  @param [in] pdev: physical device
 *
 *  Obtains write requests from the send queue chain and writes them to ETH0.
 *  In case of a failed write, if the user requested a reply he is informed that
 *  the write failed and the write request is discarded immediately. Is then up
 *  to the user to take the necessary actions.\n
 *  If the user didn't request a reply then the write will be retried until the
 *  request times out.
 */
void
eth_writer(air_uptr_t arg)
{
    iop_physical_device_t *pdev = (iop_physical_device_t *) arg;

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

    static iop_wrapper_t *wrapper=NULL;

     uint32_t i=0;
     uint32_t reads =
        pdev->reads_per_period[air_schedule.current_schedule_index];

//    iop_debug(" :: IOP - eth-writer running!\n");

    while ((!iop_chain_is_empty(&pdev->sendqueue) || wrapper!=NULL) && i<reads)
    {
        /*no wrapper set for trasnmition?*/
        if(wrapper==NULL){
            wrapper=obtain_wrapper(&pdev->sendqueue);
            eth_fragment_packet(wrapper);
        }
        /* write to the device */
        if (eth_driver->dev.write((iop_device_driver_t *) eth_driver,
                                 wrapper) == AIR_SUCCESSFUL)
        {
            /*all fragments sent?*/
            if(iop_chain_is_empty(&wrapper->fragment_queue)){
                release_wrapper(wrapper);
                wrapper=NULL;
            }
        }
        else
        {
            /*TODO review this, beware of requeueing*/
            iop_chain_append(&error, &wrapper->node);
            iop_raise_error(HW_WRITE_ERROR);
        }
        i++;
    }

    /* re-queue failed transmissions */
    while (!iop_chain_is_empty(&error)){
        iop_wrapper_t *wrapper = obtain_wrapper(&error);

        iop_chain_append(&pdev->sendqueue, &wrapper->node);
    }
}

int packet_is_final(iop_wrapper_t *wrapper){

    iop_fragment_t *frag = (iop_fragment_t *) wrapper->fragment_queue.last;

    if((frag->header.eth_header.ipoffset[0] & 0x20)==0){
        /*no more fragments*/
     //   iop_debug("no more frags %d\n", frag->header.eth_header.ipoffset[0]);
        return 1;
    }

    return 0;
}

/**
 *  @brief Task that polls eth0 for new data packets
 *  @param [in] pdev: physical device
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
eth_reader(air_uptr_t arg)
{
    iop_physical_device_t *pdev = (iop_physical_device_t *) arg;

    if (pdev == NULL)
    {
        iop_debug(" :: IOP - eth_reader parameter cannot be NULL!\n");
        return;
    }

    /* get underlying driver */
    iop_eth_device_t *driver = (iop_eth_device_t *) pdev->driver;

   // iop_debug(" :: IOP - eth-reader running!\n");

    static iop_wrapper_t *wrapper = NULL;
    iop_wrapper_t *aux=NULL;
    uint32_t i;
    uint32_t reads = pdev->reads_per_period[air_schedule.current_schedule_index];

    for (i = 0; i < reads; ++i)
    {

        if(wrapper == NULL){
            wrapper=obtain_free_wrapper();

            /* sanity check */
            if (wrapper == NULL)
            {
                iop_raise_error(OUT_OF_MEMORY);
                break;
            }
        }

        /*auxiliary wrapper for ARP bufer swapping*/
        aux=obtain_free_wrapper();
        if (aux == NULL)
        {
            iop_raise_error(OUT_OF_MEMORY);
            break;
        }

        /*safeguard original wrapper buffer*/
        iop_buffer_t *aux_buf= aux->buffer;
        aux->buffer=wrapper->buffer;

        /* read from the device */
        if (driver->dev.read((iop_device_driver_t *) driver, wrapper) == AIR_SUCCESSFUL)
        {

            switch (eth_get_packet_type(wrapper->buffer))
            {
                /* ARP packet */
            case HTONS(ETH_HDR_ARP_TYPE): 
                /*GRETH read swaps original wrapper buffer with ARP buffer*/
                eth_send_arp_reply(driver, wrapper);
                /*restore original wrapper buffer*/
                wrapper->buffer=aux->buffer;
                break;

                /* IPv4 packet */
            case HTONS(ETH_HDR_IP_TYPE):
                if(packet_is_final(wrapper)){
                    iop_chain_append(&pdev->rcvqueue, &wrapper->node);
                    wrapper = NULL;
                }
                break;

            default:
                break;
            }
        }

        /*restore auxiliary wrapper buffer for release*/
        aux->buffer=aux_buf;
        release_wrapper(aux);
    }
}
