/*
 * COPYRIGHT (c) 2007  Gaisler Research
 * Copyright (C) 2011-2017  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE.RTEMS in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */
/**
 *  @file
 *  @brief Macros used for Spacewire bus 
 *
 *  @author Cl�udio Silva, lumm
 */

#ifndef __SPW_H__
#define __SPW_H__

#include <iop.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SPW_LINKERR_EVENT RTEMS_EVENT_0

#define MEM_READ(addr) _MEM_READ((volatile void *)(addr))
#define SPW_READ(addr) _SPW_READ((volatile void *)(addr))
#define SPW_WRITE(addr,v) *addr=v

#define SPW_REG(c,r) (c->regs->r)
#define SPW_REG_CTRL(c) SPW_REG(c,ctrl)
#define SPW_REG_STATUS(c) SPW_REG(c,status)
#define SPW_REG_NODEADDR(c) SPW_REG(c,nodeaddr)

#define SPW_CTRL_READ(c)      SPW_READ(&SPW_REG_CTRL(c))
#define SPW_CTRL_WRITE(c,v)   SPW_WRITE(&SPW_REG_CTRL(c),v)
#define SPW_STATUS_READ(c)    SPW_READ(&SPW_REG_STATUS(c))
#define SPW_STATUS_WRITE(c,v) SPW_WRITE(&SPW_REG_STATUS(c),v)

#define SPW_LINKSTATE(c) (((c) >> 21) & 0x7)

#define SPW_RXBD_LENGTH 0x1ffffff
#define SPW_RXBD_EN (1 << 25)
#define SPW_RXBD_WR (1 << 26)
#define SPW_RXBD_IE (1 << 27)

#define SPW_RXBD_EEP (1 << 28)
#define SPW_RXBD_EHC (1 << 29)
#define SPW_RXBD_EDC (1 << 30)
#define SPW_RXBD_ETR (1 << 31)

#define SPW_RXBD_ERROR (SPW_RXBD_EEP | \
                        SPW_RXBD_ETR)

#define SPW_RXBD_RMAPERROR (SPW_RXBD_EHC | SPW_RXBD_EDC)

#define SPW_TXBD_LENGTH 0xffffff

#define SPW_TXBD_EN (1 << 12)
#define SPW_TXBD_WR (1 << 13)
#define SPW_TXBD_IE (1 << 14)
#define SPW_TXBD_LE (1 << 15)

#define SPW_TXBD_ERROR (SPW_TXBD_LE)

#define SPW_CTRL_LINKDISABLED (1 << 0)
#define SPW_CTRL_LINKSTART    (1 << 1)
#define SPW_CTRL_AUTOSTART    (1 << 2)
#define SPW_CTRL_IE           (1 << 3)
#define SPW_CTRL_TI           (1 << 4)
#define SPW_CTRL_PM           (1 << 5)
#define SPW_CTRL_RESET        (1 << 6)
#define SPW_CTRL_TQ           (1 << 8)
#define SPW_CTRL_LI           (1 << 9)
#define SPW_CTRL_TT           (1 << 10)
#define SPW_CTRL_TR           (1 << 11)
#define SPW_CTRL_RE           (1 << 16)
#define SPW_CTRL_RD           (1 << 17)

#define SPW_CTRL_RC           (1 << 29)
#define SPW_CTRL_RX           (1 << 30)
#define SPW_CTRL_RA           (1 << 31)

#define SPW_STATUS_TO (1 << 0)
#define SPW_STATUS_CE (1 << 1)
#define SPW_STATUS_ER (1 << 2)
#define SPW_STATUS_DE (1 << 3)
#define SPW_STATUS_PE (1 << 4)
#define SPW_STATUS_WE (1 << 6)
#define SPW_STATUS_IA (1 << 7)
#define SPW_STATUS_EE (1 << 8)

#define SPW_DMACTRL_TXEN (1 << 0)
#define SPW_DMACTRL_RXEN (1 << 1)
#define SPW_DMACTRL_TXIE (1 << 2)
#define SPW_DMACTRL_RXIE (1 << 3)
#define SPW_DMACTRL_AI   (1 << 4)
#define SPW_DMACTRL_PS   (1 << 5)
#define SPW_DMACTRL_PR   (1 << 6)
#define SPW_DMACTRL_TA   (1 << 7)
#define SPW_DMACTRL_RA   (1 << 8)
#define SPW_DMACTRL_AT   (1 << 9)
#define SPW_DMACTRL_RX   (1 << 10)
#define SPW_DMACTRL_RD   (1 << 11)
#define SPW_DMACTRL_NS   (1 << 12)

#define SPW_PREPAREMASK_TX (SPW_DMACTRL_RXEN | SPW_DMACTRL_PS | SPW_DMACTRL_TA | SPW_DMACTRL_RA | SPW_DMACTRL_RD | SPW_DMACTRL_NS)
#define SPW_PREPAREMASK_RX (SPW_DMACTRL_TXEN | SPW_DMACTRL_PR | SPW_DMACTRL_TA | SPW_DMACTRL_RA)

typedef struct {
   unsigned int hlen;
   char *hdr;
   unsigned int dlen;
   char *data;
   unsigned int sent;
} spw_ioctl_pkt_send;

/** 
 * @brief Maintains SpW bus statistics
 */ 
typedef struct {
   unsigned int tx_link_err;
   unsigned int rx_rmap_header_crc_err;
   unsigned int rx_rmap_data_crc_err;
   unsigned int rx_eep_err;
   unsigned int rx_truncated;
   unsigned int parity_err;
   unsigned int escape_err;
   unsigned int credit_err;
   unsigned int write_sync_err;
   unsigned int disconnect_err;
   unsigned int early_ep;
   unsigned int invalid_address;
   unsigned int packets_sent;
   unsigned int packets_received;
} spw_stats;

/** 
 * @brief Structure containing the SpW configuration 
 */ 
typedef struct {
   unsigned int nodeaddr;			/** node address:Y ou have to set nodeaddr and nodemask together*/
   unsigned int destkey;			/** Destination key*/
   unsigned int clkdiv; 			/** Clock Divisor: contains both CLKDIVSTART and CLKDIVRUN */
   unsigned int rxmaxlen;			/** Maximum length allowed in packetreception */
   unsigned int timer;				/** Timer */
   unsigned int disconnect;			/** Disconnect SpW link when a error occurs*/
   unsigned int promiscuous;		/** Promiscous mode: dump the whole packet in memory without removing anything */
   unsigned int rmapen;				/** Enable RMAP core*/
   unsigned int rmapbufdis;			/** Disable RMAP buffers. Only relevant if RMAP is enabled*/
   unsigned int linkdisabled;		/** SpW link is disabled */
   unsigned int linkstart;			/** SpW link has started */

   unsigned int check_rmap_err; 	/** check incoming packets for rmap errors */
   unsigned int rm_prot_id; 		/** remove protocol id from incoming packets */ 
   unsigned int tx_blocking;		/** use blocking tx */
   unsigned int tx_block_on_full; 	/** block when all tx_buffers are used */
   unsigned int rx_blocking; 		/** block when no data is available */
   unsigned int disable_err; 		/** disable link automatically when link error is detected */
   unsigned int link_err_irq; 		/** generate an interrupt when link error occurs */ 
   
   unsigned int retry; 				/** Retry Transmitting a failed packet*/
   unsigned int wait_ticks; 		/** rtems_wake_after(wait_ticks)*/

   unsigned int event_id; 			/** task id that should receive link err irq event */
   
   unsigned int is_rmap;			/** This core contains a RMAP HW core*/
   unsigned int is_rxunaligned;		/** RMAP core permits unaligned accesses to memory*/
   unsigned int is_rmapcrc;			/** RMAP automatically adds CRC checksum to packets */
   
   unsigned int nodemask;			/** Node Mask*/
} spw_config;

/** 
 * @brief Structure containing the user defined part of the SpW configuration
 */ 
typedef struct {
	unsigned int nodeaddr;				/** node address:Y ou have to set nodeaddr and nodemask together*/
	unsigned int nodemask;				/** Node Mask*/
	unsigned int destkey;				/** Destination key*/
	unsigned int clkdiv; 				/** Clock Divisor: contains both CLKDIVSTART and CLKDIVRUN */
	
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
} spw_user_config;

/**
 * @brief map of the SpW core registers
 */
typedef struct {
   volatile unsigned int ctrl;			/** Control*/
   volatile unsigned int status;		/** Status and interrupt*/
   volatile unsigned int nodeaddr;		/** Node Address */
   volatile unsigned int clkdiv;		/** Clock Divisor*/
   volatile unsigned int destkey;		/** Destination key*/
   volatile unsigned int time;			/** Time register*/
   volatile unsigned int timer;			/** Timer and disconnect register*/
   volatile unsigned int pad;			/** Not Used */
   
   volatile unsigned int dma0ctrl; 		/** DMA channel control/status*/ 
   volatile unsigned int dma0rxmax;		/**	DMA channel 1 rx maximum length */
   volatile unsigned int dma0txdesc;	/** DMA channel 1 transmit descriptor table address.*/
   volatile unsigned int dma0rxdesc;	/** DMA channel 1 receive descriptor table address.*/
   
   volatile unsigned int dma0addr;		/** Not used.  Only for SPW core 2 and onwards */
   
} LEON3_SPACEWIRE_Regs_Map;

/**
 * @brief composes a RX descriptor
 */
typedef struct {
   volatile unsigned int ctrl;	/** Control Resgister*/
   volatile unsigned int *addr;	/** Pointer to data buffer*/
} SPACEWIRE_RXBD;

/**
 * @brief TX descriptor
 */
typedef struct {
   volatile unsigned int ctrl;			/** Control Register*/
   volatile unsigned int *addr_header;	/** Address to header buffer*/
   volatile unsigned int len;			/** Data control and Header */
   volatile unsigned int *addr_data;		/** Address to data buffer*/
} SPACEWIRE_TXBD;

/**
 * @brief internal per device structure for SpW devices
 *
 * This structure maintains the internal status and configuration for a SpW device
 * There will be one of these structure for each one of the SpW devices
 */
typedef struct {

   spw_config config; 			/** Core Configuration */

   unsigned int tx_all_in_use;	/** All TX destcriptors in use*/
   unsigned int tx_sent;		/** Last desciptor that was sent*/
   unsigned int tx_cur;			/** Current descriptor*/
   unsigned int rxcur;			/** Current descitptor in the RX table*/
   unsigned int rxbufcur;		/** Pointer to curren buffer offset*/
   unsigned int txdbufsize;		/** Size of the TX data buffers*/
   unsigned int txhbufsize;		/** Size of the TX header buffers*/
   unsigned int rxbufsize;		/** Size of the RX data buffers*/
   unsigned int txbufcnt;		/** Number of TX descriptors*/
   unsigned int rxbufcnt;		/** Number of RX descriptors*/

   /* statistics */
   spw_stats stat;				/** SpW core statistics*/
   
   uint8_t *ptr_rxbuf0;			/** Pointer to the beginning of the RX buffer memory area*/
   uint8_t *ptr_txdbuf0;			/** Pointer to the beginning of the TX data buffer memory area*/
   uint8_t *ptr_txhbuf0;			/** Pointer to the beginning of the TX header buffer memory area*/

   unsigned int irq;			/** IRQ*/
   int minor;					/** minor number of this device*/
   int core_ver;				/** SpW core version: 1, 2 or 3*/
   int open;					/** Was this device opened*/
   int running;					/** Is the core running*/
   unsigned int core_freq_khz;	/** Core frequency*/
   

   /* semaphores*/
   unsigned int txsp;			/** Sempahore id for write: Not used*/
   unsigned int rxsp;			/** Sempahore id for read: Not used*/

	uint8_t *bdtable;		/** Pointer to the descriptor table */
   
   SPACEWIRE_TXBD *tx;			/** Pointer to the RX descriptor table*/
   SPACEWIRE_RXBD *rx;			/** Pointer to the TX descriptor table*/

	iop_buffer_t *iop_buffers;
	uint8_t *iop_buffers_storage;

   LEON3_SPACEWIRE_Regs_Map *regs;				/** Map if this device's registers*/
} SPW_DEV;

/**
 * @name IOCTL macros 
 *
 * Macros that define the IOCTL avaliable commands
 * @{
 */
#define SPACEWIRE_IOCTRL_SET_NODEADDR        1
#define SPACEWIRE_IOCTRL_SET_RXBLOCK         2
#define SPACEWIRE_IOCTRL_SET_DESTKEY         4 
#define SPACEWIRE_IOCTRL_SET_CLKDIV          5
#define SPACEWIRE_IOCTRL_SET_TIMER           6 
#define SPACEWIRE_IOCTRL_SET_DISCONNECT      7
#define SPACEWIRE_IOCTRL_SET_PROMISCUOUS     8
#define SPACEWIRE_IOCTRL_SET_RMAPEN          9
#define SPACEWIRE_IOCTRL_SET_RMAPBUFDIS      10
#define SPACEWIRE_IOCTRL_SET_CHECK_RMAP      11
#define SPACEWIRE_IOCTRL_SET_RM_PROT_ID      12
#define SPACEWIRE_IOCTRL_SET_TXBLOCK         14
#define SPACEWIRE_IOCTRL_SET_DISABLE_ERR     15
#define SPACEWIRE_IOCTRL_SET_LINK_ERR_IRQ    16
#define SPACEWIRE_IOCTRL_SET_EVENT_ID        17
#define SPACEWIRE_IOCTRL_SET_PACKETSIZE      20
#define SPACEWIRE_IOCTRL_GET_LINK_STATUS     23
#define SPACEWIRE_IOCTRL_GET_CONFIG          25
#define SPACEWIRE_IOCTRL_GET_STATISTICS      26
#define SPACEWIRE_IOCTRL_CLR_STATISTICS      27
#define SPACEWIRE_IOCTRL_SEND                28
#define SPACEWIRE_IOCTRL_LINKDISABLE         29
#define SPACEWIRE_IOCTRL_LINKSTART           30
#define SPACEWIRE_IOCTRL_SET_TXBLOCK_ON_FULL 31
#define SPACEWIRE_IOCTRL_SET_COREFREQ        32
#define SPACEWIRE_IOCTRL_SET_CLKDIVSTART     33
#define SPACEWIRE_IOCTRL_SET_NODEMASK        34

#define SPACEWIRE_IOCTRL_START               64
#define SPACEWIRE_IOCTRL_STOP                65
/**@}*/

uint32_t spw_initialize(iop_device_driver_t *iop_dev, void *arg);

uint32_t spw_open(iop_device_driver_t *iop_dev, void *arg);

uint32_t spw_close(iop_device_driver_t *iop_dev, void *arg);

uint32_t spw_read(iop_device_driver_t *iop_dev, void *arg);

uint32_t spw_write(iop_device_driver_t *iop_dev, void *arg );

uint32_t spw_control(iop_device_driver_t *iop_dev, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __SPW_H__ */
