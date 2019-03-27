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
#include <eth_support.h>

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
                struct timeval tstart, tnow;
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
        iop_debug("uintprt_t %d\n", sizeof(uintptr_t));
	for (i = 0; i < device->rx_count; i++){

        /* map an IOP buffer to the descriptor */
        sc->rx_iop_buffer[i] = &sc->iop_buffers[device->tx_count + i];
#ifdef DBG_BUFFERS
        iop_debug(" IOP :: Mapping IO buffer %d on v_addr 0x%06x\n", i, sc->rx_iop_buffer[i]->v_addr );
#endif    
        /* get descriptor physical address*/
		sc->rxdesc[i].addr = sc->rx_iop_buffer[i]->p_addr;
		sc->rxdesc[i].ctrl = GRETH_RXD_ENABLE;
#ifdef DBG_BUFFERS
        iop_debug(" IOP :: descriptor physical address 0x%06x\n", sc->rxdesc[i].addr );
#endif

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
//        regs->ctrl |= GRETH_CTRL_RXEN | (sc->fd << 4) | (0 << 7) | (0 << 8);
	iop_debug("  HArdware init done! leaving gigabit capabilities %d\n", sc->gb);
        return RTEMS_SUCCESSFUL;
}

uint32_t greth_validate_packet(iop_eth_device_t *dev, eth_header_t *packet) {

    /*check packet type*/
    if(packet->type == HTONS(ETH_HDR_ARP_TYPE)){
        /*arp packet found*/
        return 1;
    }    

    if(packet->type == HTONS(ETH_HDR_IP_TYPE)){
        /* check if the packet was for us */
        if (!eth_compare_mac((uint16_t*)dev->mac, (uint16_t*)packet->dst_mac) ||
                          !eth_compare_ip((uint16_t *)dev->ip, (uint16_t*)packet->dst_ip)){
      //      iop_debug("invalid packet - wrong address\n");
            return -1;
        }

        /* check the if IP version is valid */
        if (packet->vhl != 0x45) {
       //     iop_debug("invalid packet - ipv\n");
            return -1;
        }

        /* check if the packet is an UDP packet */
        if (packet->proto != IPV4_HDR_PROTO) {
       //     iop_debug("invalid packet - not udp\n");
            return -1;
        }

        /* check if the checksum is valid */
        if (eth_ipv4_chksum(packet) != (uint16_t)0xFFFF) {
       //     iop_debug("invalid packet - wrong checksum\n");
            return -1;
        }

        return 0;
    }
    
  //  iop_debug("other packet %d\n", packet->type );
    return -1;
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
static int greth_hw_receive(greth_softc_t *sc, iop_wrapper_t *wrapper, iop_eth_device_t *device){
	
    uint32_t len = 0, status = 0, error=0;
        
    /* read if the descriptor is not enabled */
    while(!((status = sc->rxdesc[sc->rx_ptr].ctrl) & GRETH_RXD_ENABLE)){

        /*Check for errors*/
        if ((status & GRETH_RXD_TOOLONG) || (status & GRETH_RXD_DRIBBLE) ||
            (status & GRETH_RXD_CRCERR)  || (status & GRETH_RXD_OVERRUN) ||
            (status & GRETH_RXD_LENERR)  || 
            (status & 0x7FF) < sizeof(eth_header_t)- sizeof(ethproto_header_t)) {
                error = 1;
                printk("IO :: error greth recieve %d desc %d len %d %d 0x%06x\n", 
                            status,sc->rx_ptr, (status & 0x7FF), sc->regs->status, sc->rxdesc[sc->rx_ptr].addr);

        }
            
        if(error==0){
            iop_debug("desc %d 0x%06x len %d\n",sc->rx_ptr, sc->rxdesc[sc->rx_ptr].addr, status & 0x7FF );
            /* get packet length */
            len = status & 0x7FF;
                
            /*get free fragment*/
            iop_fragment_t *frag = obtain_free_fragment();
            if(frag==NULL){
                /*no more free fragments*/
                iop_raise_error(OUT_OF_MEMORY);
                return -1;
            } 

                
            /*get packet header*/
            memcpy(&frag->header, sc->rx_iop_buffer[sc->rx_ptr]->v_addr, sizeof(eth_header_t));

            int valid = -1;

            if((valid = greth_validate_packet(device, &frag->header.eth_header)) == 0){
                /*IP packet udp handle packet fragments*/

                unsigned int pack_seq = (((unsigned int)frag->header.eth_header.ipoffset[0] & 0x1f)<<8) + frag->header.eth_header.ipoffset[1];
                
                /*is there a sequence number?*/
                if(pack_seq == 0){ 
                    
                    if(!iop_chain_is_empty(&wrapper->fragment_queue)){
                        /*iop_chain already had fragments from old uncomplete packet, release them.*/
                        iop_fragment_t *frag_aux;
                        while(!iop_chain_is_empty(&wrapper->fragment_queue)){
                            frag_aux = obtain_fragment(&wrapper->fragment_queue);
                            release_fragment(frag_aux);
                        }  
                                                                                                                                }

                    
                    /*set descriptor info to wrapper*/
                    memcpy(wrapper->buffer->v_addr, sc->rx_iop_buffer[sc->rx_ptr]->v_addr, 1520); /*TODO define on 1520*/
                    frag->header_size = sizeof(eth_header_t);
                    frag->payload = wrapper->buffer->v_addr + frag->header_size;
                    frag->payload_size = len-frag->header_size; 
                         
                    iop_chain_append(&wrapper->fragment_queue, &frag->node);
                         
                    wrapper->buffer->header_off=0;
                    wrapper->buffer->header_size = frag->header_size;
                    wrapper->buffer->payload_off= frag->header_size;
                    wrapper->buffer->payload_size = frag->payload_size;

                    iop_debug("seq 0 desc %d %d\n", sc->rx_ptr, frag->header.eth_header.ipoffset[0]);

                }else{
                    /*packet with sequence number in order?*/
                    if(iop_chain_is_empty(&wrapper->fragment_queue)){
                        /*No chain, assume out of order packet*/
                        iop_debug("seq outof order desc %d %d\n", sc->rx_ptr, pack_seq);
                        release_fragment(frag);
                        len = 0;
                    }else{
                        iop_fragment_t *last_frag= wrapper->fragment_queue.last;
                        unsigned int last_pack_seq = (((unsigned int)last_frag->header.eth_header.ipoffset[0] & 0x1f)<<8) + last_frag->header.eth_header.ipoffset[1];

                        if(last_pack_seq + 185 != pack_seq){ /*TODO magic number 185*/
                            /*wrong sequence number trash this fragment*/
                            iop_debug("seq wrong desc %d %d", sc->rx_ptr, last_pack_seq);
                            release_fragment(frag);
                            len=0;
                        }else{

                            /*At this point we have a fragmented packet with correct sequence number*/
                    
                            frag->header_size = sizeof(eth_header_t) - sizeof(ethproto_header_t);
                            frag->payload = last_frag->payload + last_frag->payload_size;
                            frag->payload_size = len - frag->header_size; 
                    
                            /*set descriptor info to wrapper*/
                            memcpy(frag->payload, sc->rx_iop_buffer[sc->rx_ptr]->v_addr+frag->header_size, frag->payload_size); /*TODO define on 1520*/
                            wrapper->buffer->payload_size += frag->payload_size;
                            iop_chain_append(&wrapper->fragment_queue, &frag->node);
                            iop_debug("seq correct desc %d %d %d\n", sc->rx_ptr, wrapper->buffer->payload_size, pack_seq);
                    
                        }
                    }
                }
            }else{
                /*is packet arp?*/
                if(valid == 1){
                    iop_debug("arp packet desc %d\n", sc->rx_ptr);
                    /*set ARP wrapper: ARP wrapper is only released after ARP reply has been sent*/
                    static iop_wrapper_t *arp_wrapper=NULL;
                    if(arp_wrapper==NULL)
                        arp_wrapper=obtain_free_wrapper();
                    else{
                        release_wrapper(arp_wrapper);
                        arp_wrapper=obtain_free_wrapper();
                    }

                    /*set descriptor info to wrapper*/
                   memcpy(arp_wrapper->buffer->v_addr, sc->rx_iop_buffer[sc->rx_ptr]->v_addr, 1520); /*TODO define on 1520*/
                                        
                    arp_wrapper->buffer->header_off=0;
                    arp_wrapper->buffer->header_size = sizeof(eth_header_t);
                    arp_wrapper->buffer->payload_off= sizeof(eth_header_t);
                    arp_wrapper->buffer->payload_size = len;

                    /*swap original wrapper buffer for ARP wrapper buffer*/
                    wrapper->buffer=arp_wrapper->buffer;
                                    
                    /*release unused fragment*/
                    release_fragment(frag);

                }else{
                    iop_debug("invalid desc %d %d\n", sc->rx_ptr, wrapper->buffer->payload_size);
                    release_fragment(frag);
                    len=0;
                }

            }
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

        if(error)
            return -1;

        return len;
    }
    return 0;
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

    /* get the size of the packet to send */
    uint16_t len = 0;
    
    /*sanity check*/
    if(iop_chain_is_empty(&wrapper->fragment_queue)){
        iop_debug("fragment chain empty\n");
        return -1; /*chango to error tag*/
    }
    
    /*fetch next fragment on wrapper to send*/
    iop_fragment_t *frag=obtain_fragment(&wrapper->fragment_queue);
    
    
    /* check if there are descriptor available */
    while (sc->txdesc[sc->tx_ptr].ctrl & GRETH_TXD_ENABLE){
        /* Are we allowed to block?*/
        if (sc->tx_blocking) {

             /* wait a moment for any RX descriptors to get available */
            rtems_task_wake_after(sc->wait_ticks);
        } else {
            /* We can't block waiting, so we return */
            iop_debug("can't block\n");
            return 0;
        }

    }

    len=frag->header_size + frag->payload_size;

    /*Copy header to descriptor*/
    memcpy(sc->tx_iop_buffer[sc->tx_ptr]->v_addr, &frag->header, frag->header_size );

    /*Copy payload to descriptor*/
    memcpy(sc->tx_iop_buffer[sc->tx_ptr]->v_addr+frag->header_size, frag->payload, frag->payload_size );
    iop_debug("TX - size %d %d 0x%06x desc %d\n", frag->header_size, frag->payload_size, frag->payload, sc->tx_ptr);
    /* enable descriptor*/
    if (sc->tx_ptr < sc->txbufs - 1) {

        /* re enable descriptor and write total data length*/
        sc->txdesc[sc->tx_ptr].ctrl = GRETH_TXD_ENABLE | len;
    } else {

        /* this is last descriptor so also activate WRAP */
        sc->txdesc[sc->tx_ptr].ctrl = GRETH_TXD_WRAP | GRETH_TXD_ENABLE | len ;
        iop_debug("TX - LAST DESCRIPTOR %d\n", sc->tx_ptr);
    }

    /* enable transmission */
    sc->regs->ctrl = sc->regs->ctrl | GRETH_CTRL_TXEN;
    
    /* increment descriptor */
    sc->tx_ptr = (sc->tx_ptr + 1) % sc->txbufs;
    
    release_fragment(frag);
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
                iop_debug("drivernot started\n");
		return RTEMS_NOT_CONFIGURED;
	}

	/* sanity check */
	if (wrapper == NULL || wrapper->buffer == NULL){
                iop_debug("insane\n");
		return RTEMS_INVALID_NAME;
	}

	/* While we have read no data*/
	while ((count = greth_hw_receive(sc, wrapper, device)) == 0) {
		
		/* Are we allowed to block?*/
		if (sc->rx_blocking) {
		
			/* wait a moment for any RX descriptors to get available */
			rtems_task_wake_after(sc->wait_ticks);
		} else {
		
			/* We can't block waiting, so we return */
			return RTEMS_RESOURCE_IN_USE;
		}
        }
        if(count<0){
            iop_debug("read error\n");         
            return RTEMS_INTERNAL_ERROR;
        }
       
	return RTEMS_SUCCESSFUL;
}

uint32_t greth_reset(iop_device_driver_t *iop_dev){
        /**
         *   *  Reset the controller.
         *       */
    printf("GRETH_RESET!!!\n");
    clock_gating_disable(&ambapp_plb, GATE_ETH0);
    printf("GRETH0 clock disabled\n");
    clock_gating_enable(&ambapp_plb, GATE_ETH0);
iop_eth_device_t *device = (iop_eth_device_t *)iop_dev;
               
            /*Initialize the eth core*/
          greth_initialize_hardware(device);

    printf("GRETH0 initialized\n");



#if 0
     clock_gating_enable(&ambapp_plb, GATE_ETH0);

        greth_softc_t *sc = (greth_softc_t *)device->dev.driver;
        int i=0;

        sc->regs->ctrl = 0;
        sc->regs->ctrl = GRETH_CTRL_RST;    /* Reset ON */
        sc->regs->ctrl = 0;

        /* Initialize rx/tx descriptor pointers */
        sc->tx_ptr = 0;
        sc->tx_dptr = 0;
        sc->tx_cnt = 0;
       sc->rx_ptr = 0;

        /* align the TX and RX descriptor tables */
        sc->txdesc = (greth_rxtxdesc *)(((uintptr_t)sc->txdesc + 1024) & ~(1024 - 1));
        sc->rxdesc = (greth_rxtxdesc *)((((uintptr_t)sc->rxdesc + 1024) & ~(1024 - 1)) + 1024);

        /* insert the descriptor table address in the HW register*/
        sc->regs->txdesc = (uint32_t)air_syscall_get_physical_addr((uintptr_t)sc->txdesc);
        sc->regs->rxdesc = (uint32_t)air_syscall_get_physical_addr((uintptr_t)sc->rxdesc);

        /* setup IOP buffers */
        setup_iop_buffers(sc->iop_buffers,sc->iop_buffers_storage,device->rx_count + device->tx_count);

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
       sc->regs->mac_addr_msb = device->mac[0] << 8  | device->mac[1];
       sc->regs->mac_addr_lsb = device->mac[2] << 24 | device->mac[3] << 16 |
           device->mac[4] << 8  | device->mac[5];

       /* clear status*/
       sc->regs->status = 0xffffffff;
                                                                                
       /* enable RX and setup speeds and duplex*/
       sc->regs->ctrl |= GRETH_CTRL_RXEN | (sc->fd << 4) | (sc->sp << 7) | (sc->gb << 8);
#endif
        return 1;
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
        iop_debug(" greth_write :: arguments inconsistent wrapper 0x%06x buffer 0x%06x paylaod %d\n", wrapper, wrapper->buffer, wrapper->buffer->payload_size);
        return RTEMS_INVALID_NAME;
    }

    greth_hw_send(sc, wrapper);
	return RTEMS_SUCCESSFUL;
}
