/*
 * Copyright (C) 2011  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief CPSW driver functions definitions
 */

#ifndef __IOP_CPSW_H__
#define __IOP_CPSW_H__

#include <iop.h>
#include <stdint.h>

#define CPSW0_SS_REGS                           SOC_CPSW_SS_REGS
#define CPSW0_MDIO_REGS                         SOC_CPSW_MDIO_REGS
#define CPSW0_WR_REGS                           SOC_CPSW_WR_REGS
#define CPSW0_CPDMA_REGS                        SOC_CPSW_CPDMA_REGS
#define CPSW0_ALE_REGS                          SOC_CPSW_ALE_REGS
#define CPSW0_CPPI_RAM_REGS                     SOC_CPSW_CPPI_RAM_REGS
#define CPSW0_PORT_0_REGS                       SOC_CPSW_PORT_0_REGS
#define CPSW0_PORT_1_REGS                       SOC_CPSW_PORT_1_REGS
#define CPSW0_SLIVER_1_REGS                     SOC_CPSW_SLIVER_1_REGS
#define CPSW0_PORT_2_REGS                       SOC_CPSW_PORT_2_REGS
#define CPSW0_SLIVER_2_REGS                     SOC_CPSW_SLIVER_2_REGS
#define CPSW0_PORT_1_PHY_ADDR                   0
#define CPSW0_PORT_1_PHY_GIGABIT                FALSE
#define MAX_SLAVEPORT_PER_INST                  2

#define PORT_1                                   0x0
#define PORT_2                                   0x1
#define PORT_0_MASK                              0x1
#define PORT_1_MASK                              0x2
#define PORT_2_MASK                              0x4
#define HOST_PORT_MASK                           PORT_0_MASK
#define SLAVE_PORT_MASK(slv_port_num)            (1 << slv_port_num)
#define PORT_MASK                                (0x7)
#define INDV_PORT_MASK(slv_port_num)             (1 << slv_port_num)

#define SIZE_CPPI_RAM                           0x2000
#define MDIO_FREQ_INPUT                         125000000
#define MDIO_FREQ_OUTPUT                        1000000

#define ALE_MAX_ENTRIES                         (1024)
#define ALE_ENTRY_TYPE                          0x30
#define ALE_ENTRY_TYPE_IDX                      7
#define ALE_ENTRY_FREE                          0
#define ALE_ENTRY_NUM_WORDS                     (3)
#define ALE_ENTRY_UCAST                          0x10
#define ALE_ENTRY_MCAST                          0xD0
#define ALE_UCAST_ENTRY_TYPE                     7
#define ALE_UCAST_ENTRY_DLR_PORT_BLK_SEC         8
#define ALE_UCAST_ENTRY_PORT_SHIFT               2
#define ALE_MCAST_ENTRY_TYPE_FWD_STATE           7
#define ALE_MCAST_ENTRY_PORTMASK_SUP             8
#define ALE_MCAST_ENTRY_PORTMASK_SHIFT           2
#define ALE_ETHARP_HWADDR_LEN     6

#define CPDMA_BD_OWNER                          0x20000000
#define CPDMA_BD_SOP                            0x80000000
#define CPDMA_BD_EOP                            0x40000000
#define CPDMA_BD_EOQ                            0x10000000
#define CPDMA_BD_LEN_MASK                       0xFFFF
#define CPDMA_BD_PKTLEN_MASK                    0xFFFF

#define MAX_DESCRIPTORS              ((SIZE_CPPI_RAM >> 1) / sizeof(cpdma_bd_t))


/**
 * @brief RX Buffer descriptor data structure
 */
typedef struct __cpdma_bd {

    volatile struct __cpdma_bd *next;
    volatile uint32_t bufptr;
    volatile uint32_t bufoff_len;
    volatile uint32_t flags_pktlen;
    volatile iop_buffer_t *iop_buffer;

} cpdma_bd_t;

/**
 * @brief Helper struct to hold the data used to operate on the receive
 *        buffer descriptor ring
 */
typedef struct {

    volatile cpdma_bd_t *free_head;
    volatile cpdma_bd_t *recv_head;
    volatile cpdma_bd_t *recv_tail;
    volatile uint32_t free_num;

} rxch;

/**
 * @brief Helper struct to hold the data used to operate on the transmit
 *        buffer descriptor ring
 */
typedef struct {

    volatile cpdma_bd_t *free_head;
    volatile cpdma_bd_t *send_head;
    volatile cpdma_bd_t *send_tail;
    volatile uint32_t free_num;

} txch;



/**
 * @brief Device port
 */
typedef struct {

    uint32_t port_base;             /**< */
    uint32_t sliver_base;           /**< */
    uint32_t phy_addr;              /**< */
    uint32_t phy_gbps;              /**< */

} cpsw_port_t;

/**
 * @brief
 */
typedef struct {

    uint32_t ss_base;               /**< */
    uint32_t mdio_base;             /**< */
    uint32_t wrpr_base;             /**< */
    uint32_t ale_base;              /**< */
    uint32_t cpdma_base;            /**< */
    uint32_t cppi_ram_base;         /**< */
    uint32_t host_port_base;        /**< */

    /* Slave port information */
    cpsw_port_t port[MAX_SLAVEPORT_PER_INST];

    /* Rx, and Tx channels                                      */
    txch txch;                      /**< Tx channel             */
    rxch rxch;                      /**< Rx channel             */

    /* configuration */
    uint32_t started;
    iop_buffer_t *iop_buffers;
    uint8_t *iop_buffers_storage;

} cpsw_device_t;


air_status_code_e cpsw_initialize(iop_device_driver_t *iop_dev, void *arg);
air_status_code_e cpsw_open(iop_device_driver_t *iop_dev, void *arg);
air_status_code_e cpsw_close(iop_device_driver_t *iop_dev, void *arg);
air_status_code_e cpsw_read(iop_device_driver_t *iop_dev, void *arg);
air_status_code_e cpsw_write(iop_device_driver_t *iop_dev, void *arg);

#endif  /* __IOP_CPSW_H__ */
