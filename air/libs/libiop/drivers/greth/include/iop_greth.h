/** 
 * @file IOPgreth.h
 * 
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT 
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * @ingroup Drivers
 * @author Cl�udio Silva
 * @brief GRETH ethernet driver macros and definitions
 * @note adapted from Gaisler Research ethernet MAC driver
 *
 * Interrupts and dependency from the RTEMS BSD TCP/IP stack were removed.
 * Access to the driver is now done through the usual RTEMS IO access points.
 */ 

#ifndef __IOP_GRETH_H__
#define __IOP_GRETH_H__

#include <iop.h>
#include <eth_support.h>

#define GRETH_TOTAL_BD           128	/**< Max number of decriptors*/
#define GRETH_MAXBUF_LEN         1520	/**< Ethernet max frame size*/
                                
/**
 * @name TX Descriptor
 * @attention some of these are not from greth but from greth_gbit
 */                     
#define GRETH_TXD_ENABLE      0x0800 	/**< Tx BD Enable */
#define GRETH_TXD_WRAP        0x1000 	/**< Tx BD Wrap (last BD) */
#define GRETH_TXD_IRQ         0x2000 	/**< Tx BD IRQ Enable */
#define GRETH_TXD_MORE        0x20000 	/**< Tx BD More (more descs for packet) */
#define GRETH_TXD_IPCS        0x40000 	/**< Tx BD insert ip chksum */
#define GRETH_TXD_TCPCS       0x80000 	/**< Tx BD insert tcp chksum */
#define GRETH_TXD_UDPCS       0x100000 	/**< Tx BD insert udp chksum */

#define GRETH_TXD_UNDERRUN    0x4000 	/**< Tx BD Underrun Status */
#define GRETH_TXD_RETLIM      0x8000 	/**< Tx BD Retransmission Limit Status */
#define GRETH_TXD_LATECOL     0x10000	/**< Tx BD Late Collision */

#define GRETH_TXD_STATS       (GRETH_TXD_UNDERRUN            | \
                               GRETH_TXD_RETLIM              | \
                               GRETH_TXD_LATECOL)

#define GRETH_TXD_CS          (GRETH_TXD_IPCS            | \
                               GRETH_TXD_TCPCS           | \
                               GRETH_TXD_UDPCS)
                                
/**
 * @name RX Descriptor
 * @attention some of these are not from greth but from greth_gbit
 */                       
#define GRETH_RXD_ENABLE      0x0800 	/**< Rx BD Enable */
#define GRETH_RXD_WRAP        0x1000 	/**< Rx BD Wrap (last BD) */
#define GRETH_RXD_IRQ         0x2000 	/**< Rx BD IRQ Enable */

#define GRETH_RXD_DRIBBLE     0x4000 	/**< Rx BD Dribble Nibble Status */                                
#define GRETH_RXD_TOOLONG     0x8000 	/**< Rx BD Too Long Status */
#define GRETH_RXD_CRCERR      0x10000 	/**< Rx BD CRC Error Status */
#define GRETH_RXD_OVERRUN     0x20000 	/**< Rx BD Overrun Status */
#define GRETH_RXD_LENERR      0x40000 	/**< Rx BD Length Error */
#define GRETH_RXD_ID          0x40000 	/**< Rx BD IP Detected */
#define GRETH_RXD_IR          0x40000 	/**< Rx BD IP Chksum Error */
#define GRETH_RXD_UD          0x40000 	/**< Rx BD UDP Detected*/
#define GRETH_RXD_UR          0x40000 	/**< Rx BD UDP Chksum Error */
#define GRETH_RXD_TD          0x40000 	/**< Rx BD TCP Detected */
#define GRETH_RXD_TR          0x40000 	/**< Rx BD TCP Chksum Error */


#define GRETH_RXD_STATS       (GRETH_RXD_OVERRUN             | \
                               GRETH_RXD_DRIBBLE             | \
                               GRETH_RXD_TOOLONG             | \
                               GRETH_RXD_CRCERR)

/**
 * @name CTRL Register 
 */
#define GRETH_CTRL_TXEN         0x00000001 /**< Transmit Enable */
#define GRETH_CTRL_RXEN         0x00000002 /**< Receive Enable  */
#define GRETH_CTRL_TXIRQ        0x00000004 /**< Transmit Enable */
#define GRETH_CTRL_RXIRQ        0x00000008 /**< Receive Enable  */
#define GRETH_CTRL_FULLD        0x00000010 /**< Full Duplex */
#define GRETH_CTRL_PRO          0x00000020 /**< Promiscuous (receive all) */
#define GRETH_CTRL_RST          0x00000040 /**< Reset MAC */

/**
 * @name Status Register 
 */
#define GRETH_STATUS_RXERR      0x00000001 /**< Receive Error */
#define GRETH_STATUS_TXERR      0x00000002 /**< Transmit Error IRQ */
#define GRETH_STATUS_RXIRQ      0x00000004 /**< Receive Frame IRQ */
#define GRETH_STATUS_TXIRQ      0x00000008 /**< Transmit Error IRQ */
#define GRETH_STATUS_RXAHBERR   0x00000010 /**< Receiver AHB Error */
#define GRETH_STATUS_TXAHBERR   0x00000020 /**< Transmitter AHB Error */

/**
 * @name  MDIO Control 
 * MDIO is used to access MII registers from a given PHY.
 */
#define GRETH_MDIO_WRITE        0x00000001 /**< MDIO Write */
#define GRETH_MDIO_READ         0x00000002 /**< MDIO Read */
#define GRETH_MDIO_LINKFAIL     0x00000004 /**< MDIO Link failed */
#define GRETH_MDIO_BUSY         0x00000008 /**< MDIO Link Busy */
#define GRETH_MDIO_REGADR       0x000007C0 /**< Register Address */
#define GRETH_MDIO_PHYADR       0x0000F800 /**< PHY address */
#define GRETH_MDIO_DATA         0xFFFF0000 /**< MDIO DATA */

/**
 * @name  MII extended registers 
 * @attention These are RFC standard registers; Defined somehwere in a RFC std
 */
#define GRETH_MII_EXTADV_1000FD 0x00000200
#define GRETH_MII_EXTADV_1000HD 0x00000100
#define GRETH_MII_EXTPRT_1000FD 0x00000800
#define GRETH_MII_EXTPRT_1000HD 0x00000400

/**
 * @name  MII registers 
 * @attention These are RFC standard registers; Defined somehwere in a RFC std
 */
#define GRETH_MII_100T4         0x00000200
#define GRETH_MII_100TXFD       0x00000100
#define GRETH_MII_100TXHD       0x00000080
#define GRETH_MII_10FD          0x00000040
#define GRETH_MII_10HD          0x00000020


/** @brief structure used to configure the GRETH hardware*/
typedef struct greth_configuration_s{
	uint32_t 	txd_count;			/**< Number of TX buffers*/
	uint32_t 	rxd_count;			/**< Number of RX buffers*/
	uint8_t		hw_addr[6];			/**< MAC address of this interface*/
	int accept_broadcast;			/**< NOT USED*/
} greth_configuration_t;

/**
 * @brief GR_Ethernet configuration registers 
 */
typedef struct _greth_regs {
	volatile uint32_t ctrl;         /**< Ctrl Register */
	volatile uint32_t status;       /**< Status Register */
	volatile uint32_t mac_addr_msb; /**< Bit 47-32 of MAC address */
	volatile uint32_t mac_addr_lsb; /**< Bit 31-0 of MAC address */
	volatile uint32_t mdio_ctrl;    /**< MDIO control and status */
	volatile uint32_t txdesc;       /**< Transmit descriptor pointer */
	volatile uint32_t rxdesc;       /**< Receive descriptor pointer */
} greth_regs;


/**
 * @brief PHY data
 * obtained from MII through MDIO
 */
struct phy_device_info {
	int vendor;
	int device;
	int rev;
   
	int adv;
	int part;
	
	/*extended regs*/
	int extadv;
	int extpart;
};

/**
 * @brief RX and TX Descriptor entry
 */
typedef struct _greth_rxtxdesc {
    volatile uint32_t ctrl;     /** Length and status */
    uint32_t *addr;             /** Buffer pointer */
} greth_rxtxdesc;

/**
 * @brief internal driver per device structure
 */
typedef struct greth_softc{

   greth_regs *regs;                /** Device's Registers*/

   int started;                     /** Started? */
   int rx_blocking;                 /** blocking behaviour */
   int tx_blocking;
   int wait_ticks;                  /** wait ticks for blocking*/

   int acceptBroadcast;             /** Accept Broadcast IPs*/

   unsigned int tx_ptr;
   unsigned int tx_dptr;
   unsigned int tx_cnt;
   unsigned int rx_ptr;

   unsigned int txbufs;             /** Number of TX descriptors*/
   unsigned int rxbufs;             /** Number of RX descriptors*/

   greth_rxtxdesc *txdesc;          /** TX descrioptor table*/
   greth_rxtxdesc *rxdesc;          /** RX descrioptor table*/

   /*Status*/
   struct phy_device_info phydev;   /** PHY information*/
   int fd;                          /** Full duplex*/
   int sp;                          /** Speed: 0 = 10Mbits; 1 = 100*/
   int gb;                          /** Gbit PHY*/
   int gbit_mac;                    /** Gbit MAC*/
   int auto_neg;                    /** Autonegotiation available*/
   unsigned int auto_neg_time;      /** time it took to complete autonegotiation*/

   /* configuration */
   iop_buffer_t *iop_buffers;
   uint8_t *iop_buffers_storage;

   iop_buffer_t **tx_iop_buffer;    /**< mapping of TX descriptor and IOP buffers */
   iop_buffer_t **rx_iop_buffer;    /**< mapping of RX descriptor and IOP buffers */

} greth_softc_t;

/**
 * @brief Initializes the Greth Core
 * @param iop_dev Device configuration
 * @param arg not used
 * @return RTEMS_INTERNAL_ERROR if no greth device was found.
 */
uint32_t greth_initialize(iop_device_driver_t *iop_dev, void *arg);

/**
 * @brief Prepares the device for communication
 * @param iop_dev Device configuration
 * @param arg not used
 * @return RTEMS_NOT_CONFIGURED if the device was not initialized
 */
uint32_t greth_open(iop_device_driver_t *iop_dev, void *arg);

/**
 * @brief Stops the device
 * @param iop_dev Device configuration
 * @param arg not used
 * @return RTEMS_NOT_CONFIGURED if the device was not initialized
 */
uint32_t greth_close(iop_device_driver_t *iop_dev, void *arg);

/**
 * @brief Reads from the device
 * @param iop_dev Device configuration
 * @param [in]  arg : struct greth_args with read buffer
 * @return Status of the operation:
 *         RTEMS_NOT_CONFIGURED if the device was not initialized
 *         RTEMS_INVALID_NAME if the user argument (void *arg) is inconsistent
 *         RTEMS_RESOURCE_IN_USE there is no data to be read and we can't block
 *         RTEMS_SUCCESSFUL otherwise
 **/
uint32_t greth_read(iop_device_driver_t *iop_dev, void *arg);

/**
 * @brief Writes in the device
 * @param iop_dev Device configuration
 * @param [in]  arg : struct greth_args with write buffer
 * @return Status of the operation:
 *         RTEMS_NOT_CONFIGURED if the device was not initialized
 *         RTEMS_INVALID_NAME if the user argument (void *arg) is inconsistent
 *         RTEMS_RESOURCE_IN_USE there is no data to be read and we can't block
 *         RTEMS_SUCCESSFUL otherwise
 **/
uint32_t greth_write(iop_device_driver_t *iop_dev, void *arg);
uint32_t greth_reset(iop_device_driver_t *iop_dev);

#endif /* __IOP_GRETH_H__ */
