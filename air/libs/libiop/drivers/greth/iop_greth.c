/** 
 * @file IOPgreth.c
 * 
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT 
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE. 
 *
 * @ingroup Drivers
 * @author cdcs
 * @brief GRETH Ethernet Driver.
 * @note adapted from Gaisler Research ethernet MAC driver
 *
 * Interrupts and dependency from the RTEMS BSD TCP/IP stack were removed.
 * Access to the driver is now done through the usual RTEMS IO access points.
 *
 */

#define GRETH_SUPPORTED
#include <bsp.h>
#include <air.h>
#include <rtems.h>
#include <amba.h>
#include <ambapp.h>
#include <stdint.h> 
#include <string.h>
#include <iop_greth.h>
#include <eth_support.h>
#include <ambaext.h>
#include <iop_error.h>

//#define AUTONEG_ENABLED

/** 10s Autonegotiation Timeout */
#ifndef GRETH_AUTONEGO_TIMEOUT_MS
#define GRETH_AUTONEGO_TIMEOUT_MS 3000
#endif

#define GRETH_INIT_TIMEOUT_S 3

/** 
 * @brief Read a PHY register using MDIO
 * @param phy_addr Address of the target PHY
 * @param reg_addr Offset of the target register
 * @return register contents; 0 if there was an error
 */
static uint32_t read_mii(
        greth_softc_t *greth, uint32_t phy_addr, uint32_t reg_addr){

    /*return value*/
    uint32_t rv;
    time_t tstart, tnow;

    tstart =rtems_clock_get_uptime_seconds();
    /* wait for MDIO to be ready */
    while (greth->regs->mdio_ctrl & GRETH_MDIO_BUSY) {
        tnow=rtems_clock_get_uptime_seconds();

        /*Check if we surpasses allowed time*/
        if ( (tnow-tstart) > GRETH_INIT_TIMEOUT_S ){
            iop_debug("    greth - failed to read default value after %d msec\n",(tnow-tstart) );
            return 0;
        }
    }

    /* write phyaddr and reg offset to MDIO ctrl */
    greth->regs->mdio_ctrl =
            (phy_addr << 11) | (reg_addr << 6) | GRETH_MDIO_READ;

    tstart =rtems_clock_get_uptime_seconds();
    /* wait for MDIO to ready */
    while (greth->regs->mdio_ctrl & GRETH_MDIO_BUSY) {
        tnow=rtems_clock_get_uptime_seconds();

        /*Check if we surpasses allowed time*/
        if ( (tnow-tstart) > GRETH_INIT_TIMEOUT_S ){
            iop_debug("    greth - failed to read default value after %d msec\n",(tnow-tstart) );
            return 0;
        }
    }

    /* check if there was an error */
    if (!(greth->regs->mdio_ctrl & GRETH_MDIO_LINKFAIL))

        /* obtain register contents (16 bits) */
        rv = ((greth->regs->mdio_ctrl >> 16) & 0xFFFF);

    /* we had an error */
    else {
        iop_debug("    greth - failed to read MMI\n");
        rv = 0;
    }

    return rv;
}

/** 
 * @brief write to a PHY register using MDIO
 * @param phy_addr Address of the target PHY
 * @param reg_addr Offset of the target register
 * @param data Data to be written to the register
 */
static void write_mii(
        greth_softc_t *greth, uint32_t phy_addr,
        uint32_t reg_addr, uint32_t data){
    
    time_t tstart, tnow;

    tstart =rtems_clock_get_uptime_seconds();
	/* wait for MDIO  to be ready */
    while (greth->regs->mdio_ctrl & GRETH_MDIO_BUSY) {
        tnow=rtems_clock_get_uptime_seconds();

        /*Check if we surpasses allowed time*/
        if ( (tnow-tstart)  > GRETH_INIT_TIMEOUT_S ){
            iop_debug("    greth - failed to write MMI - timeout1\n");
            return;
        }
    }
	
	/* set up the  write operation in ctrl register */
    greth->regs->mdio_ctrl =
            ((data & 0xFFFF) << 16) | (phy_addr << 11) |
            (reg_addr << 6) | GRETH_MDIO_WRITE;

    tstart =rtems_clock_get_uptime_seconds();
    /* wait for MDIO  to be ready */
    while (greth->regs->mdio_ctrl & GRETH_MDIO_BUSY) {
        tnow=rtems_clock_get_uptime_seconds();

        /*Check if we surpasses allowed time*/
        if ( (tnow-tstart)  > GRETH_INIT_TIMEOUT_S ){
            iop_debug("    greth - failed to write MMI - timeout2\n");
            return;
        }
    }

}

/** 
 * @brief initialize GRETH core
 * @param device Ethernet device configuration pointer
 */
static int greth_initialize_hardware(iop_eth_device_t *device){
	
	int i;
    int phyaddr;
    int phyctrl;
    int phystatus;
    int tmp1;
    int tmp2;
    greth_regs *regs;
    unsigned int msecs;
	
	greth_softc_t *sc = (greth_softc_t *)device->dev.driver;


    iop_debug("    GRETH hardware initialization (0%08x)\n", sc->regs);

	regs = sc->regs;
	/**
	 *	Reset the controller.
	 */
    regs->ctrl = 0;
    regs->ctrl = GRETH_CTRL_RST;	/* Reset ON */
    regs->ctrl = 0;					/* Reset OFF */
	
	 /* Check if mac is gbit capable*/
    sc->gbit_mac = (regs->ctrl >> 27) & 1;  
    
    /* Get the phy address which assumed to have been set
       correctly with the reset value in hardware*/
    phyaddr = (regs->mdio_ctrl >> 11) & 0x1F;
    
    /* get phy control register default values */
    phyctrl = read_mii(sc, phyaddr, 0);
    if((phyctrl & 0x8000) || (phyctrl == 0))
        return RTEMS_IO_ERROR;
    

    /* reset PHY and wait for completion */
    write_mii(sc, phyaddr, 0, 0x8000 | phyctrl);
    
    phyctrl = read_mii(sc, phyaddr, 0);
    if((phyctrl & 0x8000) || (phyctrl == 0))
        return RTEMS_IO_ERROR;
	
	/* clear device's capabilities*/
    sc->gb = 0;
    sc->fd = 0;
    sc->sp = 0;
    sc->auto_neg = 0;
    sc->auto_neg_time = 0;
	
	/** 
	 *  Check if PHY is autoneg capable and then determine operating mode, 
     *  otherwise force it to 10 Mbit halfduplex 
	 */
	
	/* Is the core autonegotiating?*/
	if ((phyctrl >> 12) & 1) {
	
		/* we are capable of autoneg*/
		msecs = 0;
		sc->auto_neg = 1;
		
	#ifdef AUTONEG_ENABLED
		/* try to get current time*/
		//if ( rtems_clock_get(RTEMS_CLOCK_GET_TIME_VALUE,&tstart) == RTEMS_NOT_DEFINED){ // use for RTEMS 4.
		if ( rtems_clock_get_tod_timeval(&tstart) == RTEMS_NOT_DEFINED){ // use for RTEMS 5
			/* Not inited, set to epoch */
			rtems_time_of_day time;
			time.year   = 1988;
			time.month  = 1;
			time.day    = 1;
			time.hour   = 0;
			time.minute = 0;
			time.second = 0;
			time.ticks  = 0;
			rtems_clock_set(&time);

			tstart.tv_sec = 0;
			tstart.tv_usec = 0;
            
			/* rtems_clock_get(RTEMS_CLOCK_GET_TIME_VALUE,&tstart); */ // Use for RTEMS 4.8
			rtems_clock_get_tod_timeval(&tstart); // Use for RTEMS 5
		}
	#endif

		/*wait for auto negotiation to complete*/
		while (!(((phystatus = read_mii(sc, phyaddr, 1)) >> 5) & 1)) {
		
		#ifdef AUTONEG_ENABLED

			/*Get current time*/
			/* rtems_clock_get(RTEMS_CLOCK_GET_TIME_VALUE,&tnow); // For RTEMS 4.8 */
            rtems_clock_get_tod_timeval(&tnow); // For RTEMS 5
			
			/*calculate deltat between now and tstart*/
			msecs = (tnow.tv_sec-tstart.tv_sec)*1000+(tnow.tv_usec-tstart.tv_usec)/1000;
			// /*Check if we surpasses autoneg time*/
			if ( msecs > GRETH_AUTONEGO_TIMEOUT_MS ){
				
				/*autoneg timed out*/
				sc->auto_neg_time = msecs;
				iop_debug("    Auto negotiation timed out. Selecting default config\n");
				
				/** @todo i don't like this. why use reg from before reset???*/
				tmp1 = read_mii(sc, phyaddr, 0);
				
				/* gbit?*/
				sc->gb = ((phyctrl >> 6) & 1) && !((phyctrl >> 13) & 1);
				
				/*speed: 10 or 100 mbit?*/
				sc->sp = !((phyctrl >> 6) & 1) && ((phyctrl >> 13) & 1);
				
				/*full duplex?*/
				sc->fd = (phyctrl >> 8) & 1;
				
				goto auto_neg_done;
			}
		#endif
		}
		
		/*autonegotiation completed within time*/
		sc->auto_neg_time = msecs;
		
		/*read autoneg results*/
		sc->phydev.adv = read_mii(sc, phyaddr, 4);
		sc->phydev.part = read_mii(sc, phyaddr, 5);
		
		/* extended status register available*/
		if ((phystatus >> 8) & 1) {
		
			/* read regs*/
			sc->phydev.extadv = read_mii(sc, phyaddr, 9);
			sc->phydev.extpart = read_mii(sc, phyaddr, 10);
			
			/* Check gbit capabilities: FD=FullDuplex HD=HalfDuplex */
		    if ( (sc->phydev.extadv & GRETH_MII_EXTADV_1000FD) &&
				(sc->phydev.extpart & GRETH_MII_EXTPRT_1000FD)) {
				   sc->gb = 1;
				   sc->fd = 1;
		    }
		    if ( (sc->phydev.extadv & GRETH_MII_EXTADV_1000HD) &&
				(sc->phydev.extpart & GRETH_MII_EXTPRT_1000HD)) {
				   sc->gb = 1;
				   sc->fd = 0;
		    }
		}
		
		/*If we are not gbit*/
		if ((sc->gb == 0) || ((sc->gb == 1) && (sc->gbit_mac == 0))) {
			
			/*Check capabilities: FD=FullDuplex HD=HalfDuplex */
			if ( (sc->phydev.adv & GRETH_MII_100TXFD) &&
				 (sc->phydev.part & GRETH_MII_100TXFD)) {
					sc->sp = 1;
					sc->fd = 1;
			}
			if ( (sc->phydev.adv & GRETH_MII_100TXHD) &&
				 (sc->phydev.part & GRETH_MII_100TXHD)) {
					sc->sp = 1;
					sc->fd = 0;
			}
			if ( (sc->phydev.adv & GRETH_MII_10FD) &&
				 (sc->phydev.part & GRETH_MII_10FD)) {
					sc->fd = 1;
			}
		}
    }


#ifdef AUTONEG_ENABLED
auto_neg_done:
#endif
    iop_debug("    auto negotiation done\n");

	/* zero out device information*/
    sc->phydev.vendor = 0;
    sc->phydev.device = 0;
    sc->phydev.rev = 0;
	
	/* read PHY status register*/
    phystatus = read_mii(sc, phyaddr, 1);
    
    /* Read out PHY info if extended registers are available */
    if (phystatus & 1) {
		
		/* these registers contain PHY information*/
		tmp1 = read_mii(sc, phyaddr, 2);
		tmp2 = read_mii(sc, phyaddr, 3);
		
		/*obtain vendor dev and revision information*/
		sc->phydev.vendor = (tmp1 << 6) | ((tmp2 >> 10) & 0x3F);
		sc->phydev.rev = tmp2 & 0xF;
		sc->phydev.device = (tmp2 >> 4) & 0x3F;
    }

    /* Force to 10 mbit half duplex if the 10/100 MAC is used with a 1000 PHY*/
    /* check if marvell 88EE1111 PHY. Needs special reset handling */
    if ((phystatus & 1) && (sc->phydev.vendor == 0x005043) && (sc->phydev.device == 0x0C)) {
		if (((sc->gb) && !(sc->gbit_mac)) || !((phyctrl >> 12) & 1)) {
			/*select speed!*/
			write_mii(sc, phyaddr, 0, sc->sp << 13);
			write_mii(sc, phyaddr, 0, 0x8000);
			sc->gb = 0;
			sc->sp = 0;
			sc->fd = 0;
		}
    } else {
		if (((sc->gb) && !(sc->gbit_mac))  || !((phyctrl >> 12) & 1)) {
			/*select speed!*/
			write_mii(sc, phyaddr, 0, sc->sp << 13);
			sc->gb = 0;
			sc->sp = 0;
			sc->fd = 0;
		}
    }
    
	/*wait for PHY to be ready*/
    phyctrl = read_mii(sc, phyaddr, 0);
    if((phyctrl & 0x8000) || (phyctrl == 0))          
        return RTEMS_IO_ERROR;

	
	/**
	 *	Reset the controller.
	 */
    regs->ctrl = 0;
    regs->ctrl = GRETH_CTRL_RST;	/* Reset ON */
    regs->ctrl = 0;

    /* Initialize rx/tx descriptor pointers */
    sc->tx_ptr = 0;
    sc->tx_dptr = 0;
    sc->tx_cnt = 0;
    sc->rx_ptr = 0;

    /* align the TX and RX descriptor tables */
    sc->txdesc = (greth_rxtxdesc *)(((uintptr_t)sc->txdesc + 1024) & ~(1024 - 1));
    sc->rxdesc = (greth_rxtxdesc *)((((uintptr_t)sc->rxdesc + 1024) & ~(1024 - 1)) + 1024);

	/* insert the descriptor table address in the HW register*/
    regs->txdesc = (uint32_t)air_syscall_get_physical_addr((uintptr_t)sc->txdesc);
    regs->rxdesc = (uint32_t)air_syscall_get_physical_addr((uintptr_t)sc->rxdesc);

    /* setup IOP buffers */
    setup_iop_buffers(
            sc->iop_buffers,
            sc->iop_buffers_storage,
            device->rx_count + device->tx_count);

	/* setup TX descriptors */
	for (i = 0; i < device->tx_count; i++){

		/* clear descriptor control*/
	    sc->txdesc[i].ctrl = 0;

	    /* map an IOP buffer to the descriptor */
	    sc->tx_iop_buffer[i] = &sc->iop_buffers[i];

	    /* get descriptor physical address*/
		sc->txdesc[i].addr = sc->tx_iop_buffer[i]->p_addr;
   }

    /* setup RX descriptors */
	for (i = 0; i < device->rx_count; i++){

        /* map an IOP buffer to the descriptor */
        sc->rx_iop_buffer[i] = &sc->iop_buffers[device->tx_count + i];

	    /* get descriptor physical address*/
		sc->rxdesc[i].addr = sc->rx_iop_buffer[i]->p_addr;
		sc->rxdesc[i].ctrl = GRETH_RXD_ENABLE;
    }

	/* active wrap bit in last table descriptor*/
    sc->rxdesc[sc->rxbufs - 1].ctrl |= GRETH_RXD_WRAP;
	
	/* set our MAC address */
    regs->mac_addr_msb = device->mac[0] << 8  | device->mac[1];
    regs->mac_addr_lsb = device->mac[2] << 24 | device->mac[3] << 16 |
                         device->mac[4] << 8  | device->mac[5];

    /* clear status*/
    regs->status = 0xffffffff;
	
	/* enable RX and setup speeds and duplex*/
	regs->ctrl |= GRETH_CTRL_RXEN | (sc->fd << 4) | (sc->sp << 7) | (sc->gb << 8);
	iop_debug("  HArdware init done! leaving\n");
        return RTEMS_SUCCESSFUL;
}

/** 
 *  \param [in]  sc	: internal driver structure
 *  \param [in]  b : pointer to header
 *  \param [in]  d : pointer to data
 *
 *  \return number of bytes read
 *
 *  \brief reads data from Ethernet
 **/
static int greth_hw_receive(greth_softc_t *sc, iop_wrapper_t *wrapper){
	
	uint32_t len = 0, status = 0, error;

	/* read while the descriptor is not enabled */
	while (!((status = sc->rxdesc[sc->rx_ptr].ctrl) & GRETH_RXD_ENABLE)) {
	    /* reset length and error flag */
		len = error = 0;

		/*Check for errors*/
		if ((status & GRETH_RXD_TOOLONG) || (status & GRETH_RXD_DRIBBLE) ||
		    (status & GRETH_RXD_CRCERR)  || (status & GRETH_RXD_OVERRUN) ||
		    (status & GRETH_RXD_LENERR)  || (status & 0x7FF) < offsetof(eth_header_t, src_port)) {
            error = 1;
		}
		
		/* did we had and error during reception? */
		if (error == 0) {

			/* get packet length */
            len = status & 0x7FF;
            /* swap IOP buffers */
            iop_buffer_t *temp = wrapper->buffer;
            wrapper->buffer = sc->rx_iop_buffer[sc->rx_ptr];
            sc->rx_iop_buffer[sc->rx_ptr] = temp;

            /* replace pointer in the descriptor */
            sc->rxdesc[sc->rx_ptr].addr = sc->rx_iop_buffer[sc->rx_ptr]->p_addr;

            /* setup offsets */
            wrapper->buffer->header_off = 0;
            wrapper->buffer->header_size = offsetof(eth_header_t, src_port);
            wrapper->buffer->payload_off = offsetof(eth_header_t, src_port);
            wrapper->buffer->payload_size = len - offsetof(eth_header_t, src_port);
		}
		
		/* re-activate descriptor */
		if (sc->rx_ptr == sc->rxbufs - 1) {
			/* this is the last descriptor so enable it and activate WRAP*/
			sc->rxdesc[sc->rx_ptr].ctrl = GRETH_RXD_ENABLE | GRETH_RXD_WRAP;
		} else {
			/* just enable the descriptor */
			sc->rxdesc[sc->rx_ptr].ctrl = GRETH_RXD_ENABLE;
		}

        /* increment descriptor */
        sc->rx_ptr = (sc->rx_ptr + 1) % sc->rxbufs;

		/*enable reception*/
		sc->regs->ctrl |= GRETH_CTRL_RXEN;

		/* if we read something*/
		if (len != 0) {
			break;
		}
	}
	
	return len;
}

/** 
 *  \param [in]  sc	: internal driver structure
 *  \param [in]  wrapper : wrapper with buffer to tx
 *
 *  \return number of bytes written
 *
 *  \brief writes data to ethernet
 **/
static int greth_hw_send(greth_softc_t *sc, iop_wrapper_t *wrapper){

    static uint8_t buffer[14 + 65535 + 44*(14+20)];/*eth_head + Max_data_tx + (max_frags*eth+IP )*/
    /* get the size of the packet to send */
    uint16_t len = (uint16_t)get_buffer_size(wrapper->buffer);
    uint16_t lenght, frags;
    uint8_t *ptr = buffer;

    /* fragment */
    if(len > 1514)
    {
        len = eth_fragment_packet(wrapper, buffer);

        frags = len/1514;
        if (len % 1514)
            frags += 1;

        if(frags > sc->txbufs)
        {
            iop_debug("    GRETH Tx not send. Increase descriptors count\n");
            return -1;
        }

    }
    else
        memcpy(buffer, get_header(wrapper->buffer), len);

    while (len)
    {
        /* check if there are descriptor available */
        while (sc->txdesc[sc->tx_ptr].ctrl & GRETH_TXD_ENABLE){

            /* Are we allowed to block?*/
            if (sc->tx_blocking) {

                /* wait a moment for any RX descriptors to get available */
                rtems_task_wake_after(sc->wait_ticks);
            } else {

                /* We can't block waiting, so we return */
                return 0;
            }
        }

        if(len > 1514)
            lenght = 1514;
        else
            lenght = len;

        /* replace pointer in the descriptor */
        sc->txdesc[sc->tx_ptr].addr = (uint32_t *)air_syscall_get_physical_addr((uintptr_t)ptr);

         /* swap IOP buffers */
          iop_buffer_t *temp = wrapper->buffer;
          wrapper->buffer = sc->tx_iop_buffer[sc->tx_ptr];
          sc->tx_iop_buffer[sc->tx_ptr] = temp;
//         /* replace pointer in the descriptor */
//         sc->txdesc[sc->tx_ptr].addr =
//                 (uint32_t *)((uintptr_t)temp->p_addr + temp->header_off);

        /* enable descriptor*/
        if (sc->tx_ptr < sc->txbufs - 1) {

            /* re enable descriptor and write total data length*/
            sc->txdesc[sc->tx_ptr].ctrl = GRETH_TXD_ENABLE | lenght;
        } else {

            /* this is last descriptor so also activate WRAP */
            sc->txdesc[sc->tx_ptr].ctrl =
                    GRETH_TXD_WRAP | GRETH_TXD_ENABLE | lenght;
        }

        /* enable transmission */
        sc->regs->ctrl = sc->regs->ctrl | GRETH_CTRL_TXEN;

        /* increment descriptor */
        sc->tx_ptr = (sc->tx_ptr + 1) % sc->txbufs;

        len -= lenght;
        ptr += lenght;
    }

    /* return number of bytes written */
    return (int)(ptr - buffer);
}


uint32_t greth_initialize(iop_device_driver_t *iop_dev, void *arg) {
	
	/* Enable Eth Clock gate */
        clock_gating_enable(&ambapp_plb, GATE_ETH0);
    
        int device_found = 0;
	rtems_status_code status = RTEMS_SUCCESSFUL;
	struct ambapp_apb_info apbgreth;
	
	iop_eth_device_t *device = (iop_eth_device_t *)iop_dev;
	struct greth_softc *sc = (struct greth_softc *)(device->dev.driver);

	memset(&apbgreth, 0, sizeof(struct ambapp_apb_info));
	
	/* Scan for MAC AHB slave interface */
	device_found = ambapp_find_apbslv(&ambapp_plb, VENDOR_GAISLER, GAISLER_ETHMAC,
									&apbgreth);

	if (device_found != 1){
	    iop_debug("    GRETH device not found...\n");
		return RTEMS_INTERNAL_ERROR;
	}
	
	if (status == RTEMS_SUCCESSFUL){

		/* Store configuration parameters */
		sc->txbufs = device->tx_count;
		sc->rxbufs = device->rx_count;
		
		sc->regs = (void *)apbgreth.start;
		
		sc->acceptBroadcast = 0;
		sc->tx_blocking = 0;
		sc->rx_blocking = 0;
		sc->wait_ticks = 1;
		
		/* Device was innited */
		sc->started = 1;
	}
	
	return status;
}

uint32_t greth_open(iop_device_driver_t *iop_dev, void *arg) {

	rtems_status_code status = RTEMS_SUCCESSFUL;
    iop_eth_device_t *device = (iop_eth_device_t *)iop_dev;
    struct greth_softc *sc = (struct greth_softc *)(device->dev.driver);

	if(!sc->started)
		status = RTEMS_NOT_CONFIGURED;
	
	/*Initialize the eth core*/
	status= greth_initialize_hardware(device);

	return status;
}


uint32_t greth_close(iop_device_driver_t *iop_dev, void *arg) {

    iop_eth_device_t *device = (iop_eth_device_t *)iop_dev;
    struct greth_softc *sc = (struct greth_softc *)(device->dev.driver);
	
	if(!sc->started)
		return RTEMS_NOT_CONFIGURED;
	
	/**
	 *	Reset the controller.
	 */
	sc->regs->ctrl = 0;		        	/* RX/TX OFF */
    sc->regs->ctrl = GRETH_CTRL_RST;	/* Reset ON */
    sc->regs->ctrl = 0;	         		/* Reset OFF */
	
	return RTEMS_SUCCESSFUL;
}


uint32_t greth_read(iop_device_driver_t *iop_dev, void *arg) {

    int count;
    iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;
    iop_eth_device_t *device = (iop_eth_device_t *)iop_dev;
    greth_softc_t *sc = (greth_softc_t *)device->dev.driver;
	
    /* check if driver was initialized */
	if(!sc->started) {
		return RTEMS_NOT_CONFIGURED;
	}

	/* sanity check */
	if (wrapper == NULL || wrapper->buffer == NULL){
		return RTEMS_INVALID_NAME;
	}

	/* While we have read no data*/
	while ((count = greth_hw_receive(sc, wrapper)) == 0) {
		
		/* Are we allowed to block?*/
		if (sc->rx_blocking) {
		
			/* wait a moment for any RX descriptors to get available */
			rtems_task_wake_after(sc->wait_ticks);
		} else {
		
			/* We can't block waiting, so we return */
			return RTEMS_RESOURCE_IN_USE;
		}
	}

	return RTEMS_SUCCESSFUL;
}


uint32_t greth_write(iop_device_driver_t *iop_dev, void *arg) {

    iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;
    iop_eth_device_t *device = (iop_eth_device_t *)iop_dev;
    greth_softc_t *sc = (greth_softc_t *)device->dev.driver;

	if(!sc->started)
		return RTEMS_NOT_CONFIGURED;
		
    /* Verify user arguments consistency*/
    if (wrapper == NULL ||
        wrapper->buffer == NULL ||
        wrapper->buffer->payload_size == 0){
        return RTEMS_INVALID_NAME;
    }

    greth_hw_send(sc, wrapper);
	return RTEMS_SUCCESSFUL;
}
