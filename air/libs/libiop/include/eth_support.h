/**
 * @file
 * @author pfnf
 * @brief Ethernet support structures and function definitions
 */

#ifndef __ETH_SUPPORT_H__
#define __ETH_SUPPORT_H__

#include <iop.h>
#include <rtems.h>
#include <iop_support.h>

/**
 * @brief Ethernet device
 */
typedef struct {

    iop_device_driver_t dev;        /**< Generic device configuration   */
    uint8_t ip[4];                  /**< Device IP address              */
    uint8_t mac[6];                 /**< Device MAC address             */
    uint16_t rx_count;              /**< Device RX descriptor count     */
    uint16_t tx_count;              /**< Device TX descriptor count     */
//    uint16_t type;                  /**< Protocol type UDP/TCP          */

} iop_eth_device_t;

/**
 * @brief The Ethernet header.
 */
typedef struct {

    uint8_t dst_mac[6];             /**< destination ethernet address   */
    uint8_t src_mac[6];             /**< source ethernet address        */
    uint16_t type;                  /**< next protocol type             */

} __attribute__((packed)) ethII_header_t;

/**
 * @brief IP Ethernet packet type
 */
#define ETH_HDR_IP_TYPE                         ((uint16_t)0x0800)
/**
 * @brief ARP Ethernet packet type
 */
#define ETH_HDR_ARP_TYPE                        ((uint16_t)0x0806)

/**
 * @brief ARP - Address Resolution Protocol Packet
 */
typedef struct {

    ethII_header_t eth_hdr;         /**< Ethernet header                */
    uint16_t h_type;                /**< Hardware type                  */
    uint16_t p_type;                /**< Protocol type                  */
    uint8_t h_len;                  /**< Hardware address length        */
    uint8_t p_len;                  /**< Protocol address length        */
    uint16_t oper;                  /**< Operation code                 */
    uint8_t sha[6];                 /**< Sender Hardware address        */
    uint8_t spa[4];                 /**< Sender Protocol address        */
    uint8_t tha[6];                 /**< Target Hardware address        */
    uint8_t tpa[4];                 /**< Target Protocol address        */

} __attribute__((packed)) arp_packet_t ;

/**
 * @brief Computes the ipv4 packet checksum
 * @param buf Buffer with the packet data
 * @return Checksum of the packet
 */
uint16_t eth_ipv4_chksum(uint8_t *buf);
/**
 * @brief Pre-build the IP header
 * @param buf pointer to the Ethernet header
 */
void eth_prebuild_header(eth_header_t *buf);
/**
 * @brief Compare two ethernet headers
 * @param wrapper Received packet
 * @param header Router header
 * @return true if header match, false otherwise
 */
uint32_t eth_compare_header(iop_wrapper_t *wrapper, iop_header_t *header);
/**
 * @brief Copy and Complete ethernet header
 * @param iop_dev IO device
 * @param wrapper IOP wrapper with the packet
 * @param header route header
 */
void eth_copy_header(
        iop_physical_device_t *iop_dev,
        iop_wrapper_t *wrapper,
        iop_header_t *header);
/**
 * @brief Validate received packet
 * @param iop_dev IO device
 * @param wrapper IOP wrapper with the packet
 * @return 1 new packet received, 0 otherwise
 */
uint32_t eth_validate_packet(
        iop_eth_device_t *dev, iop_wrapper_t *wrapper);
/**
 * @brief Fragment a wrapper
 * @param wrapper IOP wrapper with the packet to be fragmented
 * @param buffer auxiliary buffer that will be filled with all data to be transmitted
 * @return size of data in buffer to be transmit
 */
uint32_t eth_fragment_packet(iop_wrapper_t *wrapper, uint8_t *buf);

/**
 * @brief Get the type of ethernet packet
 * @param buf Buffer pointer with the packet
 * @return packet type
 */
#define eth_get_packet_type(buf) \
        (((ethII_header_t *) get_header((buf)) )->type)

/**
 *  @brief Task that writes pending write requests to ETH0
 *  @param [in] arg: not used
 *
 *  Obtains write requests from the sendqueue chain and writes them to ETH0.
 *  In case of a failed write, if the user requested a reply he is informed that
 *  the write failed and the write request is discarded immediately. Is then up
 *  to the user to take the necessary actions.\n
 *  If the user didn't request a reply then the write will be retried until the
 *  request times out.
 */
void eth_writer();

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
void eth_reader();

#endif /* __ETH_SUPPORT_H__ */
