/**
 *  @file 
 *  @brief Header for GR1553 driver Remote Terminal mode
 *
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT
 *
 *	@note adapted from Gaisler's GR1553RT driver
 *  Modifications: 
 *    - Back port to RTEMS 4.8 (no drvmgr)
 *    - Removed Interrupts
 *    - Full revamp.
 *
 *  @author Cláudio Silva
 *	
 */
/*  GR1553B RT driver
 *
 *  COPYRIGHT (c) 2010.
 *  Aeroflex Gaisler.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  2010-03-15, Daniel Hellstrom <daniel@gaisler.com>
 *    Created
 *
 */
 
#ifndef __GR1553RT_H__
#define __GR1553RT_H__

/** 
 * @brief Descriptor read/written by hardware.
 * @note Must be aligned to 16 byte boundary 
 */
struct gr1553rt_bd {
	volatile uint32_t ctrl;		/**< 0x00 Control/Status word */
	volatile uint32_t dptr;		/**< 0x04 Data Pointer */
	volatile uint32_t next;		/**< 0x08 Next Descriptor in list */
	volatile uint32_t unused;	/**< 0x0C UNUSED BY HARDWARE */
};

/**
 * @brief Sub address table entry, the hardware access 
 */
struct gr1553rt_sa {
	volatile uint32_t ctrl;	    /**< 0x00 SUBADDRESS CONTROL WORD */
	volatile uint32_t txptr;	/**< 0x04 TRANSMIT BD POINTER */
	volatile uint32_t rxptr;	/**< 0x08 RECEIVE BD POINTER */
	volatile uint32_t unused;	/**< 0x0C UNUSED BY HARDWARE */
};

/** 
 * @brief initializes the core to operate has a remote terminal
 * @param grb_priv [in] internal device structure
 */
void gr1553rt_device_init(grb_priv *priv);

uint32_t gr1553rt_write(iop_device_driver_t *iop_dev, void *arg);

uint32_t gr1553rt_read(iop_device_driver_t *iop_dev, void *arg);

#endif 
