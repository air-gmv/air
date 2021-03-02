/*
 * Copyright (C) 2011  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/AIR/raw/master/air/LICENSE
 */

#include <iop.h>
#include <iop_mms.h>
#include <iop_support.h>
#include <iop_error.h>
#include <iop_functions.h>
#include <taste_api.h>

/**
 * @fn iop_init_devs(void)
 * @return
 * @brief Initializes the free wrappers chain queues
 */
static void iop_init_queues(air_u32_t remote_port_max_size){

    iop_debug(" :: IOP - initializing queues (%i)\n",
              usr_configuration.wrappers_count);

    air_u32_t i;
    air_u32_t size = remote_port_max_size;


    /* setup Remote Ports buffers */
    for (i = 0; i < usr_configuration.wrappers_count; ++i) {
        /* get virtual and physical addresses for this buffer, align to doubleword */
        usr_configuration.iop_buffers[i].v_addr = (void *)(((air_u32_t) &usr_configuration.iop_buffers_storage[i * (size+94+(0x08-1))] + (0x08-1)) & ~(0x08-1)); //add max total space needed eth header (TCP) = 14+20+60 TODO remove 94 and use something proper

        usr_configuration.iop_buffers[i].p_addr = (void *)air_syscall_get_physical_addr((air_uptr_t) usr_configuration.iop_buffers[i].v_addr);
    }

    /* append buffers to the wrappers */
    for (i = 0; i < usr_configuration.wrappers_count; ++i) {
        usr_configuration.wrappers[i].buffer = &usr_configuration.iop_buffers[i];
#ifdef DBG_BUFFERS
        iop_debug(" IOP :: Wrapper %d on v_addr 0x%06x\n", i, usr_configuration.wrappers[i].buffer->v_addr);
#endif
        /*initialize wrapper fragment queue*/
        iop_chain_initialize_empty(&usr_configuration.wrappers[i].fragment_queue);
    }

    /* initialize chain of empty wrappers*/
    iop_chain_initialize(
            &usr_configuration.free_wrappers,
            (void *)usr_configuration.wrappers,
            usr_configuration.wrappers_count, sizeof(iop_wrapper_t));

    /* setup fragment queue*/
    iop_chain_initialize(
            &usr_configuration.free_fragments,
            (void *)usr_configuration.fragments,
            usr_configuration.fragment_count, sizeof(iop_fragment_t));

 }

/**
 * @fn iop_init_devs(void)
 * @return
 * @brief Initializes physical and logical devices.
 */
void iop_init_devs(iop_logical_device_t *ldev, iop_physical_device_t *pdev) {
        int j=0;

        /* initialize logical device chains without any elements */
        iop_chain_initialize_empty(&ldev->sendqueue);
        iop_chain_initialize_empty(&ldev->rcvqueue);

        /* initialize headers based on the corresponding physical device*/
        for (j = 0; j < ldev->routes.length; ++j) {

            /* route pointer */
            iop_logical_route_t *route = get_logical_route(ldev, j);
            iop_debug(" :: IOP - iop_logical_route_t (%x), device %x\n",
              route, route->device);
            /* pre-build header if necessary */
            if (route->device->header_prebuild != NULL) {
                route->device->header_prebuild(route->header);
            }
        }
    

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

/**
 *  @return AIR_SUCCESSFUL if the operation was completed correctly
 *
 *  @brief Initializes IOP support features (send/receive queues, mms, configuration devices)
 *
 */

void init_iopsupport(air_u32_t remote_port_max_size){
    
   /* initialize queues */
    iop_init_queues(remote_port_max_size);
    
    /* initialize mms */
    iop_init_mms();
    
    /* initialize Devices and respective routes */
    //iop_init_devs();
         
    return;
}

/**
 *  @return AIR_SUCCESSFUL if the operation was completed correctly
 *  @param[in] dev_id Physical Device index on physical_device_list
 *  @brief Opens physical device. If not initialized, device is initialized.
 *
 */
air_status_code_e open_device(iop_physical_device_t *pdev){
    
     air_status_code_e rc = AIR_SUCCESSFUL;
     
     /* get physical device */
    iop_debug("   Physical device address 0x%04x\n", pdev);

    /* initialize device */
    if (pdev->driver->init != NULL) {
        rc = pdev->driver->init(pdev->driver, NULL);
        iop_debug("   Driver init result %d\n", rc);
    }

    /* open device */
    if (AIR_SUCCESSFUL == rc && pdev->driver->open != NULL) {
        rc = pdev->driver->open(pdev->driver, NULL);
        iop_debug("   Driver open result %d\n", rc);
    }
    
     return rc;
}


/**
 *  @return AIR_SUCCESSFUL if the operation was completed correctly
 *  @param[in] dev_id Physical Device index on physical_device_list
 *  @brief Closes physical device
 *
 */
air_status_code_e close_device(iop_physical_device_t *pdev){
    
     air_status_code_e rc = AIR_SUCCESSFUL;
     
    iop_debug("   Physical device address 0x%04x\n", pdev);

    /* close device */
    if (pdev->driver->close != NULL) {
        rc = pdev->driver->close(pdev->driver, NULL);
        iop_debug("   Driver close result %d\n", rc);
    }
    
    return rc;
}

/**
 *  @return AIR_SUCCESSFUL if the operation was completed correctly
 *  @param[in] dev_id Logical Device index on physical_device_list
 *  @param[in] route_id Logical route index on devices' logical_routes
 *  @param[in] buffer Pointer to message
 *  @param[in] len    Message length to send
 *  @brief Writes message to logical device and logical route specified.
 *
 */
air_status_code_e write_to_device(iop_logical_device_t *dev, int route_id, char* buffer, int len){
    
     air_status_code_e rc = AIR_SUCCESSFUL;
     
     
     /* get a empty request wrapper from the wrapper chain*/
     iop_wrapper_t *wrapper = obtain_free_wrapper();

    /* check if an empty wrapper was available */
    if (wrapper == NULL) {
        iop_raise_error(NO_REQUESTS);
        rc = AIR_DEVICE_ERROR;
        return rc;
    }
    
    if(len>0){
        wrapper->buffer->payload_off = sizeof(iop_header_t);
        wrapper->buffer->payload_size = len;
        wrapper->buffer->header_off = 0;
        wrapper->buffer->header_size = sizeof(iop_header_t);
        memcpy((uintptr_t)wrapper->buffer->v_addr + sizeof(iop_header_t), buffer, len);
        
        /* append data to aimed device */
        iop_logical_route_t log_route = ((iop_logical_route_t*)(&dev->routes)->elements)[route_id];
        
        /*include routing information on wrapper*/
        route_request(dev, wrapper);
        /*writer task for device driver*/
        log_route.device->writer_task((air_uptr_t)log_route.device);

    } else {
        /* release the wrapper */
        release_wrapper(wrapper);
        rc = AIR_DEVICE_ERROR;
    }
    
    return rc;
}

/**
 *  @return AIR_SUCCESSFUL if the operation was completed correctly
 *  @param[in] dev_id Physical Device index on physical_device_list
 *  @param[out] route_id Physical route index on devices' physical_routes for message received
 *  @param[out] buffer Pointer to message received
 *  @param[out] len    Message length received
 *  @brief Reads message from device.
 *
 */
air_status_code_e read_from_device(iop_physical_device_t *dev, int* route_id, char* buffer, unsigned int* len){
    
    int i=0;
     air_status_code_e rc = AIR_NO_ACTION;
     *len=0;
     dev->reader_task((air_uptr_t)dev);
     
     /* See if data was received on this device from HW */
    if (!iop_chain_is_empty(&dev->rcvqueue)){

        /* extract first request wrapper from send queue */
        iop_wrapper_t *reply_wrapper = obtain_wrapper(&dev->rcvqueue);

        /* apply routing information to this data */
        for (i = 0; i < dev->routes.length; ++i){

            /* route pointer */
            iop_physical_route_t *route =
                    &((iop_physical_route_t *)dev->routes.elements)[i];

            /* Determine if this data is for this route based on the header */
            if (dev->header_compare(reply_wrapper, route->header)) {
                *route_id = i;
                *len= get_payload_size(reply_wrapper->buffer);
                memcpy(buffer, (air_message_ptr_t)get_payload(reply_wrapper->buffer), *len);
                rc = AIR_SUCCESSFUL;
            }
            
        }
        
         /* free wrapper */
        release_wrapper(reply_wrapper);   
    }
    
    return rc;
}
