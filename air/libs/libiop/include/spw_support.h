/**
 * @file
 * @author pfnf
 * @brief Ethernet support structures and function definitions
 */

#ifndef __SPW_SUPPORT_H__
#define __SPW_SUPPORT_H__

#include <iop.h>
#include <iop_support.h>

/**
 * @brief SpaceWire device
 */
typedef struct {

    iop_device_driver_t dev;    /**< Generic device configuration   */
	unsigned int nodeaddr;		/** node address:Y ou have to set nodeaddr and nodemask together*/
	unsigned int nodemask;		/** Node Mask*/
	unsigned int destkey;		/** Destination key*/
	unsigned int clkdiv; 		/** Clock Divisor: contains both CLKDIVSTART and CLKDIVRUN */
	
	unsigned int rxmaxlen;		/** Maximum length allowed, in bytes, in packet reception */
	unsigned int promiscuous;	/** Promiscous mode: dump the whole packet in memory without removing anything */
	unsigned int rmapen;		/** Enable RMAP core*/
	unsigned int rmapbufdis;	/** Disable RMAP buffers. Only relevant if RMAP is enabled*/
	unsigned int rm_prot_id;	/** remove protocol id from incoming packets */ 
	
	unsigned int txdbufsize;	/** Transmission data buffer size*/
	unsigned int txhbufsize;	/** Transmission header buffer size*/
	unsigned int rxbufsize;		/** Reception data buffer size. Should be equal to #rxmaxlen*/
	unsigned int txdesccnt;		/** Number of TX descriptors*/
	unsigned int rxdesccnt;		/** Number of RX descriptors*/
	
	unsigned int retry;			/** Retry Transmitting a failed packet*/
	unsigned int wait_ticks;	/** Active wait: rtems_wake_after(wait_ticks)*/	
	
	unsigned int init_timeout;	/** Timeout for initialization */

} iop_spw_device_t;

/**
 * @brief SpaceWire router device
 */
typedef struct {
	
	iop_device_driver_t dev;
	unsigned int flags;
	unsigned int config;
	unsigned int iid;
	unsigned int idiv;
	unsigned int prescaler;

} iop_spw_router_device_t;

/**
 * @brief Pre-build the SPW header
 * @param buf pointer to the SPW header
 */
void spw_prebuild_header(spw_header_t *buf);
/**
 * @brief Compare two spacewire headers
 * @param wrapper Received packet
 * @param header Router header
 * @return true if header match, false otherwise
 */
uint32_t spw_compare_header(iop_wrapper_t *wrapper, iop_header_t *header);
/**
 * @brief Copy and Complete spacewire header
 * @param iop_dev IO device
 * @param wrapper IOP wrapper with the packet
 * @param header route header
 */
void spw_copy_header(
        iop_physical_device_t *iop_dev,
        iop_wrapper_t *wrapper,
        iop_header_t *header);
		
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
void spw_writer(air_uptr_t arg);

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
void spw_reader(air_uptr_t arg);

/* These do nothing */
void spwrtr_reader(air_uptr_t arg);

void spwrtr_writer(air_uptr_t arg);

#endif /* __ETH_SUPPORT_H__ */
