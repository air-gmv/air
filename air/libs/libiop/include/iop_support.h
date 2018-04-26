/** 
 * @file
 * 
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT 
 * 
 * @author cdcs
 * @brief Declares several support functions
 */

#ifndef __IOP_SUPPORT_H__
#define __IOP_SUPPORT_H__

#include <iop.h>
#include <ambapp.h>

/* Macros to set or clear a bit from a register */
#define SET_BIT_REG(adr, bit) *(volatile uint32_t *)(adr) |= (1<<bit);
#define CLEAR_BIT_REG(adr, bit) *(volatile uint32_t *)(adr) &= ~(1<<bit);

/* Available unit for clock gating */
typedef enum {
    GATE_ETH0 = 0,
    GATE_ETH1 = 1,
    GATE_SPWR = 2,
    GATE_PCI  = 3,
    GATE_1553 = 4,
	GATE_CAN  = 5
} clock_gating_device;

/**
 * @brief The Clock Gating register mapping
 */
struct clkgate_regs {
    volatile uint32_t unlock;      /**< 0x00 Unlock register */
    volatile uint32_t clock_enable;/**< 0x04 Clock enable register */
    volatile uint32_t core_reset;  /**< 0x08 Core reset register */
    volatile uint32_t override;    /**< 0x0C CPU/FPU override register */
};

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
        iop_buffer_t *buffers, uint8_t *storage, uint32_t count);

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
 * @brief Gets a IOP wrapper from a given queue
 * @param ctl Queue of IOP wrappers
 * @return NULL if no wrapper available in queue, IOP wrapper pointer otherwise
 */
iop_wrapper_t *obtain_wrapper(iop_chain_control *ctl);

/**
 * @brief Gets a free IOP wrapper
 * @return NULL if no wrapper available in queue, IOP wrapper pointer otherwise
 */
#define obtain_free_wrapper() \
    obtain_wrapper(&usr_configuration.free_wrappers)

/**
 * @brief Updates the timers on the wrappers
 * @param queue List of wrappers
 * @param timeout Wrapper timeout
 */
void update_queue_timers(iop_chain_control *queue, uint32_t timeout);

/**
 * @brief Update expiration timers
 */
void update_timers();

/**
 * @brief Enable device with clock gating
 * @param clk_amba_bus AMBA bus where the clock gating is located
 * @param core_to_enable Which device to enable.
 *        Available devices are: ETH0, ETH1, SPWR, PCI, 1553 and CAN
 */
void clock_gating_enable(amba_confarea_type* clk_amba_bus, clock_gating_device core_to_enable);

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
