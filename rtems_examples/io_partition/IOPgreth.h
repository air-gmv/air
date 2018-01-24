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
 * @author Cláudio Silva
 * @brief GRETH ethernet driver macros and definitions
 * @note adapted from Gaisler Research ethernet MAC driver
 *
 * Interrupts and dependency from the RTEMS BSD TCP/IP stack were removed.
 * Access to the driver is now done through the usual RTEMS IO access points.
 */ 

#ifndef _GR_ETH_
#define _GR_ETH_

#include <stdint.h> 
#include <rtems.h>

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



#define TXRX_COUNT              128
#define ACCEPT_BROADCAST        1



typedef struct eth_header_s {
	uint8_t 	dest_ip[4];			/**< IP of a remote target*/
	uint8_t 	dest_mac[6];		/**< MAC of the remote target / router*/
	uint16_t 	dest_port; 			/**< UDP port on the target */
	uint16_t  source_port;		/**< Source PORT*/
} eth_header_t;


/**
 * @brief RX and TX Descriptor entry 
 */
typedef struct _greth_rxtxdesc {
	volatile uint32_t ctrl; 	/** Length and status */
	uint32_t *addr;         	/** Buffer pointer */
} greth_rxtxdesc;



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
 * @brief internal driver per device structure 
 */
typedef struct greth_softc_s{

   greth_regs *regs;  				/** Device's Registers*/
   
   int started;						/** Started? */
   int rx_blocking;					/** blocking behaviour */
   int tx_blocking;
   int wait_ticks;					/** wait ticks for blocking*/
   
   int acceptBroadcast;				/** Accept Broadcast IPs*/
   
   unsigned int tx_ptr;
   unsigned int tx_dptr;
   unsigned int tx_cnt;
   unsigned int rx_ptr;
   
   unsigned int txbufs;				/** Number of TX descriptors*/
   unsigned int rxbufs;				/** Number of RX descriptors*/
   
   greth_rxtxdesc *txdesc;			/** TX descrioptor table*/
   greth_rxtxdesc *rxdesc;			/** RX descrioptor table*/
   
   /*Status*/
   struct phy_device_info phydev;	/** PHY information*/
   int fd;							/** Full duplex*/
   int sp;							/** Speed: 0 = 10Mbits; 1 = 100*/
   int gb;							/** Gbit PHY*/
   int gbit_mac;					/**	Gbit MAC*/
   int auto_neg;					/** Autonegotiation available*/
   unsigned int auto_neg_time;		/** time it took to complete autonegotiation*/
   
   /*
    * Statistics
    */
   unsigned long rxPackets;
   unsigned long rxLengthError;
   unsigned long rxNonOctet;
   unsigned long rxBadCRC;
   unsigned long rxOverrun;
   
   unsigned long txDeferred;
   unsigned long txHeartbeat;
   unsigned long txLateCollision;
   unsigned long txRetryLimit;
   unsigned long txUnderrun;

} greth_softc_t;





static uint32_t read_mii(uint32_t phy_addr, uint32_t reg_addr);
static void write_mii(uint32_t phy_addr, uint32_t reg_addr, uint32_t data);
static void greth_initialize_hardware (greth_softc_t *sc);
static int greth_hw_receive(greth_softc_t *sc, char *b, char *d);
static int sendpacket (greth_softc_t *sc, char *b, int c, char *data, int data_len);
 
rtems_device_driver greth_initialize(rtems_device_major_number  major,
								     rtems_device_minor_number  minor,
								     void *arg);
rtems_device_driver greth_open(rtems_device_major_number  major,
							   rtems_device_minor_number  minor,
							   void *arg);
rtems_device_driver greth_close(rtems_device_major_number  major,
							   rtems_device_minor_number  minor,
							   void *arg);
rtems_device_driver greth_read(rtems_device_major_number  major,
							   rtems_device_minor_number  minor,
							   void *arg);
rtems_device_driver greth_write(rtems_device_major_number  major,
							    rtems_device_minor_number  minor,
							    void *arg);
rtems_device_driver greth_ioctl(rtems_device_major_number  major,
							    rtems_device_minor_number  minor,
							    void *arg);

#endif /*_GR_ETH_*/
