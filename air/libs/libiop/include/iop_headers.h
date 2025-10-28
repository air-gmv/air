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
 * @brief Contains the header for the different device types
 */

#ifndef __IOP_HEADERS_H__
#define __IOP_HEADERS_H__

#include <stdint.h>

/**
 * @brief SPW Header
 */
typedef struct
{
    //    uint16_t hlen;              /**< Header Length */
    uint8_t hdr[32]; /**< Header */

} __attribute__((packed)) spw_header_t;

/**
 * @brief MilStd1553b header
 */
typedef struct
{

    uint8_t desc;    /**< RT Address */
    uint8_t address; /**< Subaddress */

} __attribute__((packed)) milstd_header_t;

/**
 * @brief Ethernet protocol UDP header
 *
 */
typedef struct
{
    /* UDP Specific header. */
    uint16_t udplen;    /**< udp packet length */
    uint16_t udpchksum; /**< pseudo header + UDP header + data checksum */
} __attribute__((packed)) udp_header_t;

/**
 * @brief Ethernet protocol specific header
 *
 * This header is a union of all supported specific protocol headers
 */
typedef union {

    udp_header_t udp_header;

} __attribute__((packed)) protospec_header_t;

typedef struct
{

    uint16_t src_port; /**< Source port */
    uint16_t dst_port; /**< Destination port */

    protospec_header_t specific_header;

} __attribute__((packed)) ethproto_header_t;

/**
 * @brief Eth header (Eth + IP + UDP/TCP port header)
 */
typedef struct
{
    /* EthII header */
    uint8_t dst_mac[6]; /**< destination ethernet address*/
    uint8_t src_mac[6]; /**< source ethernet address*/
    uint16_t type;      /**< next protocol type */

    /* IPv4 header*/
    uint8_t vhl;         /**< Version and Header length in 32 bits words */
    uint8_t tos;         /**< Type Of Service. Never used */
    uint16_t len;        /**< Total Lentgh: from 21 to 1499 */
    uint8_t ipid[2];     /**< Ip packet identificator. Used in reassemble */
    uint8_t ipoffset[2]; /**< Fragment Offset. Used in reassemble */
    uint8_t ttl;         /**< Time to Live. Defualts to 64*/
    uint8_t proto;       /**< Protocol: UDP = 0x11 */
    uint16_t ipchksum;   /**< IP header and data checksum */
    uint8_t src_ip[4];   /**< Source Ip address */
    uint8_t dst_ip[4];   /**< Destination IP address */

    ethproto_header_t proto_header; /** Protocol specific header**/

} __attribute__((packed)) eth_header_t;

typedef struct
{
    uint8_t extended; /**< CANBUS extended id option */
    uint8_t rtr;      /**< Remote Transmission request */
    uint8_t sshot;    /**< Single shot option*/
    uint32_t id;      /**< Remote device identifier */
} __attribute__((packed)) can_header_t;

typedef struct
{
    uint32_t id; /**< Header */
} __attribute__((packed)) uart_header_t;

/**
 * @brief SD Header.
 */
typedef struct
{
    uint32_t size;
    uint8_t file[30];
} __attribute__((packed)) sd_header_t;

/**
 * @brief ADC Header.
 */
typedef struct
{
    uint32_t id;
} __attribute__((packed)) adc_header_t;

/**
 * @brief TME Header.
 */
typedef struct
{
    uint32_t vc;
} __attribute__((packed)) tme_header_t;

/**
 * @brief GPIO Header.
 */
typedef struct
{
    uint8_t pin;
} __attribute__((packed)) gpio_header_t;


/**
 * @brief IOP generic header
 *
 * This header is a union of all supported headers
 */
typedef union {
    milstd_header_t milstd_hdr; /**< MILSTD 1553b header        */
    spw_header_t spw_header;    /**< SPW header                 */
    eth_header_t eth_header;    /**< ETH header (ETH + IP + UDP/TCP ports)*/
    can_header_t can_header;    /**< CAN frame header            */
    uart_header_t uart_header;  /**< UART header                 */
    sd_header_t sd_header;      /**< SD header                   */
    adc_header_t adc_header;    /**< ADC header                  */
    tme_header_t tme_header;    /**< TME header                  */
    gpio_header_t gpio_header;  /**< GPIO header                  */

} __attribute__((packed)) iop_header_t;

#endif /* __IOP_HEADERS_H__ */
