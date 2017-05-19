/** 
 * @file support.c
 * 
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT 
 * 
 * @author cdcs
 * @brief Provides support functions for handling requests and replies
 *
 * Defines several helper functions used across the IOPartition to manage
 * requests and replies.
 */

#include <iop.h>
#include <rtems.h>
#include <iop_mms.h>
#include <iop_chain.h>
#include <iop_error.h>
#include <iop_support.h>


void setup_iop_buffers(
        iop_buffer_t *buffers, uint8_t *storage, uint32_t count) {

    uint32_t i;
    for (i = 0; i < count; ++i) {

        /* get virtual and physical addresses for this buffer */
        buffers[i].v_addr = &storage[i * IOP_BUFFER_SIZE];
        buffers[i].p_addr = (void *)xky_syscall_get_physical_addr((uintptr_t)buffers[i].v_addr);
    }
}

void copy_iop_buffer(iop_buffer_t *dst, iop_buffer_t *src) {

    /* copy sizes and offsets */
    dst->header_off = src->header_off;
    dst->payload_off = src->payload_off;
    dst->header_size = src->header_size;
    dst->payload_size = src->payload_size;

    /* copy payload */
    memcpy((void *)((uintptr_t)dst->v_addr + dst->payload_off),
           (void *)((uintptr_t)src->v_addr + src->payload_off),
           dst->payload_size);
}

void release_wrapper(iop_wrapper_t *wrapper) {

    wrapper->timer = 0;
    iop_chain_append(&usr_configuration.free_wrappers, &wrapper->node);
}

iop_wrapper_t *obtain_wrapper(iop_chain_control *ctl) {

    iop_chain_node *node = NULL;

    if (ctl != NULL) {
        /* get the node from the chain*/
        node = iop_chain_get(ctl);
    }

    /* Return the empty wrapper*/
    return (iop_wrapper_t *)node;
}

void update_queue_timers(iop_chain_control *queue, uint32_t timeout) {

    /* pointer to the next request wrapper */
    iop_wrapper_t *curr_wrapper, *next_wrapper;

    /* sanity check */
    if (queue == NULL){
        return;
    }

    /* verify if the chain is empty */
    if (!iop_chain_is_empty(queue)) {

        /* get first wrapper on the chain */
        curr_wrapper = (iop_wrapper_t *)(iop_chain_head(queue)->next);

        do {

            /* increment timer*/
            ++curr_wrapper->timer;

            /* check if the request or reply has timed out */
            if (curr_wrapper->timer > timeout) {

                /* extract the reply from its chain */
                iop_chain_extract(&curr_wrapper->node);

                /* go for next wrapper on the chain*/
                next_wrapper = (iop_wrapper_t *)curr_wrapper->node.next;

                /* release the request wrapper */
                release_wrapper(curr_wrapper);

                /* continue processing the new wrapper */
                curr_wrapper = next_wrapper;

            } else {

                /* go for next wrapper on the chain*/
                curr_wrapper = (iop_wrapper_t *)curr_wrapper->node.next;
            }

        /* verify if we reached the end of the Chain */
        } while (curr_wrapper != (iop_wrapper_t *)iop_chain_tail(queue));
    }
}

/** 
 * @brief iterates over all requests and replies wrappers currently being
 *        used and increments the timers of those requests/replies.
 *
 * These per wrapper times avoid that an unprocessed request/reply stays
 * hovering in the IOPartition forever. If we allowed this to happen it could
 * lead to resource exhaustion (request/replys).
 */
void update_timers() {

    uint32_t i;

    iop_debug("    updating logical devices timers (%i)\n",
              usr_configuration.logical_devices.length);

    for (i = 0; i < usr_configuration.logical_devices.length; ++i) {

        /* get logical device */
        iop_logical_device_t *ldev = get_logical_device(i);

        /* update timers */
        update_queue_timers(&ldev->sendqueue, usr_configuration.time_to_live);
        update_queue_timers(&ldev->rcvqueue, usr_configuration.time_to_live);
        //update_request_timers(&ldev->pending_rcvqueue, usr_configuration.time_to_live);
    }

    iop_debug("    updating physical devices timers (%i)\n",
              usr_configuration.physical_devices.length);

    /* iterate over all physical devices */
    for (i = 0; i < usr_configuration.physical_devices.length; ++i) {

        /* get physical device */
        iop_physical_device_t *pdev = get_physical_device(i);

        /* update timers */
        update_queue_timers(&pdev->sendqueue, usr_configuration.time_to_live);
        update_queue_timers(&pdev->rcvqueue, usr_configuration.time_to_live);
    }
}

