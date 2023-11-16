/*
 * Copyright (C) 2011  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/** 
 * @file
 * 
 * @author cdcs
 * @brief Declares several support functions
 */

#ifndef __IOP_SUPPORT_H__
#define __IOP_SUPPORT_H__

#include <iop.h>

#define get_header(buf) \
    ((iop_buffer_t *)((uintptr_t)(buf)->v_addr + (buf)->header_off))

#define get_payload(buf) \
    ((uint8_t *)((uintptr_t)(buf)->v_addr + (buf)->payload_off))

#define get_buffer(buf) \
    ((uint8_t *)((uintptr_t)(buf)->v_addr + (buf)->header_off))

#define get_header_size(buf) \
    ((uint32_t)((buf)->header_size))

#define get_payload_size(buf) \
    ((uint32_t)((buf)->payload_size))

#define get_buffer_size(buf) \
    ((uint32_t)(get_header_size((buf)) + get_payload_size((buf))))

void setup_iop_buffers(
        iop_buffer_t *buffers, air_u8_t *storage, air_u32_t count);

/**
 * @brief Copies an IOP buffer
 * @param dst Destination IOP buffer
 * @param src Source IOP buffer
 */
void copy_iop_buffer(iop_buffer_t *dst, iop_buffer_t *src);

/**
 * @brief Releases a IOP wrapper back to the free queue
 * @param wrapper IOP wrapper pointer
 */
void release_wrapper(iop_wrapper_t *wrapper);

/**
 * @brief Releases a IOP fragment back to the free queue
 * @param frag IOP fragment pointer
 *    */
void release_fragment(iop_fragment_t *frag);


/**
 * @brief Gets a IOP wrapper from a given queue
 * @param ctl Queue of IOP wrappers
 * @return NULL if no wrapper available in queue, IOP wrapper pointer otherwise
 */
iop_wrapper_t *obtain_wrapper(iop_chain_control *ctl);

/**
 * @brief Gets a IOP fragment from a given queue
 * @param ctl Queue of IOP fragments
 * @return NULL if no wrapper available in queue, IOP wrapper pointer otherwise
 */
iop_fragment_t *obtain_fragment(iop_chain_control *ctl);

/**
 * @brief Gets a free IOP wrapper
 * @return NULL if no wrapper available in queue, IOP wrapper pointer otherwise
 */
#define obtain_free_wrapper() \
    obtain_wrapper(&usr_configuration.free_wrappers)

/**
 * @brief Gets a free IOP fragment
 * @return NULL if no fragment available in queue, IOP fragment pointer otherwise
 */
#define obtain_free_fragment() \
    obtain_fragment(&usr_configuration.free_fragments)


/**
 * @brief Updates the timers on the wrappers
 * @param queue List of wrappers
 * @param timeout Wrapper timeout
 */
void update_queue_timers(iop_chain_control *queue, air_u32_t timeout);

/**
 * @brief Update expiration timers
 */
void update_timers();

#define get_physical_device(i) \
    ((iop_physical_device_t **)usr_configuration.physical_devices.elements)[(i)]

#define get_logical_device(i) \
    ((iop_logical_device_t **)usr_configuration.logical_devices.elements)[(i)]

#define get_physical_route(pdev, i) \
    &((iop_physical_route_t *)(pdev)->routes.elements)[(i)]

#define get_logical_route(ldev, i) \
    &((iop_logical_route_t *)(ldev)->routes.elements)[(i)]

#define get_remote_port(i) \
    &((iop_port_t *)usr_configuration.remote_ports.elements)[(i)]


#endif /* __IOP_SUPPORT_H__ */
