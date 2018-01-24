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
 * @author Cláudio Silva
 * @brief GRETH Ethernet Driver.
 * @note adapted from Gaisler Research ethernet MAC driver
 *
 * Interrupts and dependency from the RTEMS BSD TCP/IP stack were removed.
 * Access to the driver is now done through the usual RTEMS IO access points.
 *
 */
 
 
#include <rtems.h>

#define GRETH_SUPPORTED
#include <bsp.h>


/*rtems-edi safe*/
#include <stdint.h> 
#include <string.h>

#include "IOPgreth.h"
#include "iop.h"
#include "IOPethernet.h"

/**
 * @brief Receive buffer size -- Allow for a full ethernet packet including CRC
 */
#define RBUF_SIZE 1518

//#define AUTONEG_ENABLED

/** 10s Autonegotiation Timeout */
#ifndef GRETH_AUTONEGO_TIMEOUT_MS
#define GRETH_AUTONEGO_TIMEOUT_MS 3000
#endif

/** Current device structure*/
static greth_softc_t greth;

static uint8_t desc_table[1024*((DEVICE_COUNT*2)+1)];
static uint8_t rxbufmem[GRETH_MAXBUF_LEN*TXRX_COUNT];
static uint8_t txbufmem[GRETH_MAXBUF_LEN*TXRX_COUNT];

/** 
 *  \param [in]  phy_addr	: address of the target PHY
 *  \param [in]  reg_addr	: offset of the target register
 *  \return uint32_t		: register contents; 0 if there was an error
 *
 *  \brief read a PHY register using MDIO
 **/
static uint32_t read_mii(uint32_t phy_addr, uint32_t reg_addr){	

	/*return value*/
	uint32_t rv;
	
	/*Wait for MDIO to be ready*/
	while (greth.regs->mdio_ctrl & GRETH_MDIO_BUSY) {}
	
	/*Write phyaddr and reg offset to MDIO ctrl*/
    greth.regs->mdio_ctrl = (phy_addr << 11) | (reg_addr << 6) | GRETH_MDIO_READ;
	
	/*Wait for MDIO to ready*/
    while (greth.regs->mdio_ctrl & GRETH_MDIO_BUSY) {}
	
	/*Check if there was an error*/
    if (!(greth.regs->mdio_ctrl & GRETH_MDIO_LINKFAIL))
		
		/*obtain register contents (16 bits)*/
		rv = ((greth.regs->mdio_ctrl >> 16) & 0xFFFF);
	
	/*we had an error*/
    else {
		printf("greth: failed to read mii\n");
		rv = 0;
    }
	
	return rv;
}


/** 
 *  \param [in]  phy_addr	: address of the target PHY
 *  \param [in]  reg_addr	: offset of the target register
 *  \param [in]  data		: data to be written to the register
 *
 *  \brief write to a PHY register using MDIO
 **/
static void write_mii(uint32_t phy_addr, uint32_t reg_addr, uint32_t data){
	
	/*Wait for MDIO to be ready*/
    while (greth.regs->mdio_ctrl & GRETH_MDIO_BUSY) {}
	
	/*Set up the  write operation in ctrl register*/
    greth.regs->mdio_ctrl = ((data & 0xFFFF) << 16) | (phy_addr << 11) | (reg_addr << 6) | GRETH_MDIO_WRITE;
	
	/*Wait for MDIO to be ready*/
    while (greth.regs->mdio_ctrl & GRETH_MDIO_BUSY) {}
}


/** 
 *  \param [in]  sc	: structure of the device to initialize
 *  \brief initialize greth core
 **/
static void greth_initialize_hardware (greth_softc_t *sc){
	
	int i;
    int phyaddr;
    int phyctrl;
    int phystatus;
    int tmp1;
    int tmp2;
    unsigned int msecs;
    greth_regs *regs;
	char *memarea;
	
#ifdef AUTONEG_ENABLED
    rtems_clock_time_value tstart, tnow;
#endif

    uint8_t *hw_addr;
	
	
    greth.rxPackets = 0;
	
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
    while ((phyctrl = read_mii(phyaddr, 0)) & 0x8000) {}
    
    /* reset PHY and wait for completion */
    write_mii(phyaddr, 0, 0x8000 | phyctrl);

	/* wait for reset to be complete*/
    while ((read_mii(phyaddr, 0)) & 0x8000) {}
	
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
		if ( rtems_clock_get(RTEMS_CLOCK_GET_TIME_VALUE,&tstart) == RTEMS_NOT_DEFINED){
			
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

			tstart.seconds = 0;
			tstart.microseconds = 0;
			rtems_clock_get(RTEMS_CLOCK_GET_TIME_VALUE,&tstart);
		}
	#endif

		/*wait for auto negotiation to complete*/
		while (!(((phystatus = read_mii(phyaddr, 1)) >> 5) & 1)) {
		
		#ifdef AUTONEG_ENABLED
			/*Get current time*/
			if ( rtems_clock_get(RTEMS_CLOCK_GET_TIME_VALUE,&tnow) != RTEMS_SUCCESSFUL )
				printf("rtems_clock_get failed\n\r");
			
			/*calculate deltat between now and tstart*/
			msecs = (tnow.seconds-tstart.seconds)*1000+(tnow.microseconds-tstart.microseconds)/1000;
			
			// /*Check if we surpasses autoneg time*/
			if ( msecs > GRETH_AUTONEGO_TIMEOUT_MS ){
				
				/*autoneg timed out*/
				sc->auto_neg_time = msecs;
				printf("Auto negotiation timed out. Selecting default config\n\r");
				
				/** @todo i don't like this. why use reg from before reset???*/
				tmp1 = read_mii(phyaddr, 0);
				
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
		sc->phydev.adv = read_mii(phyaddr, 4);
		sc->phydev.part = read_mii(phyaddr, 5);
		
		/* extended status register available*/
		if ((phystatus >> 8) & 1) {
		
			/* read regs*/
			sc->phydev.extadv = read_mii(phyaddr, 9);
			sc->phydev.extpart = read_mii(phyaddr, 10);
			
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
	printf("auto negotiation done\n");
	/* zero out device information*/
    sc->phydev.vendor = 0;
    sc->phydev.device = 0;
    sc->phydev.rev = 0;
	
	/* read PHY status register*/
    phystatus = read_mii(phyaddr, 1);
    
    /* Read out PHY info if extended registers are available */
    if (phystatus & 1) {
		
		/* these registers contain PHY information*/
		tmp1 = read_mii(phyaddr, 2);
		tmp2 = read_mii(phyaddr, 3);
		
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
			write_mii(phyaddr, 0, sc->sp << 13);
			write_mii(phyaddr, 0, 0x8000);
			sc->gb = 0;
			sc->sp = 0;
			sc->fd = 0;
		}
    } else {
		if (((sc->gb) && !(sc->gbit_mac))  || !((phyctrl >> 12) & 1)) {
			/*select speed!*/
			write_mii(phyaddr, 0, sc->sp << 13);
			sc->gb = 0;
			sc->sp = 0;
			sc->fd = 0;
		}
    }
	
	/*wait for PHY to be ready*/
    while ((read_mii(phyaddr, 0)) & 0x8000) {}
	
	/**
	 *	Reset the controller.
	 */
    regs->ctrl = 0;
    regs->ctrl = GRETH_CTRL_RST;	/* Reset ON */
    regs->ctrl = 0;

    /* Initialize rx/tx descriptor pointers */
    /*return the first 1024 aligned address inside desc_table memory area*/
    char *tmp = (((int)desc_table+1024) & ~(1024 -1));
    sc->txdesc = (greth_rxtxdesc *) tmp;
    sc->rxdesc = (greth_rxtxdesc *) (tmp + 1024);
    sc->tx_ptr = 0;
    sc->tx_dptr = 0;
    sc->tx_cnt = 0;
    sc->rx_ptr = 0;
	
	/* insert the descriptor table address in the HW register*/
    regs->txdesc = (int) sc->txdesc;
    regs->rxdesc = (int) sc->rxdesc;

	/* get buffer memory*/
	memarea = (char *)txbufmem;
	
	/* lets prepare tx descriptors. Iterate over all of them */
	for (i = 0; i < sc->txbufs; i++){

		
		/* clear descriptor control*/
	    sc->txdesc[i].ctrl = 0;
		
	    if (!(sc->gbit_mac)) {
			
			/* split buf memory into "txbufs" buffers*/
			sc->txdesc[i].addr =(uint32_t *) (memarea + i*GRETH_MAXBUF_LEN);
			
		}
   }
	
	/* get buffer memory*/
	memarea = (char *)rxbufmem;
	
	/* lets prepare rx descriptors. Iterate over all of them */
	for (i = 0; i < sc->rxbufs; i++){
		sc->rxdesc[i].addr = (uint32_t *)(memarea + i*GRETH_MAXBUF_LEN);
		sc->rxdesc[i].ctrl = GRETH_RXD_ENABLE;
    }
	
	/* active wrap bit in last table descriptor*/
    sc->rxdesc[sc->rxbufs - 1].ctrl |= GRETH_RXD_WRAP;
	
	/* set our MAC address */
    hw_addr = get_my_mac();
    regs->mac_addr_msb = 
		hw_addr[0] << 8 | hw_addr[1];
    regs->mac_addr_lsb = 
		hw_addr[2] << 24 | hw_addr[3] << 16 |
		hw_addr[4] << 8 | hw_addr[5];


    /* clear status*/
    regs->status = 0xffffffff;
	
	/*enable RX and setup speeds and duplex*/
	regs->ctrl |= GRETH_CTRL_RXEN | (sc->fd << 4) | (sc->sp << 7) | (sc->gb << 8);
	
	return;
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
static int greth_hw_receive(greth_softc_t *sc, char *b, char *d) {
	
	unsigned int len = 0, len_status = 0, bad;
	int exit = 0;
	uint8_t *buffer;
	
	/* read while the descirptor is not enabled */
	while (!((len_status =
		    sc->rxdesc[sc->rx_ptr].ctrl) & GRETH_RXD_ENABLE) && (!exit))
	{
		len = 0;
		bad = 0;
		/*Check for errors*/
		if (len_status & GRETH_RXD_TOOLONG)
		{
			sc->rxLengthError++;
			bad = 1;
		}
		if (len_status & GRETH_RXD_DRIBBLE)
		{
			sc->rxNonOctet++;
			bad = 1;
		}
		if (len_status & GRETH_RXD_CRCERR)
		{
			sc->rxBadCRC++;
			bad = 1;
		}
		if (len_status & GRETH_RXD_OVERRUN)
		{
			sc->rxOverrun++;
			bad = 1;
		}
		if (len_status & GRETH_RXD_LENERR)
		{
			sc->rxLengthError++;
			bad = 1;
		}
		
		/* did we had and error during reception? */
		if (!bad){	
			// printf("****************NOT BAD*****************\n");
			
			/* pass on the packet in the receive buffer */
			/* obtain packet length */
            len = len_status & 0x7FF;
			
			//printf("LEN = %d\n", len);
			
			/* cast so we can index using number of bytes */
			buffer = (uint8_t *) sc->rxdesc[sc->rx_ptr].addr;
			
			// printf("BUFFER = %s\n", &buffer[42]);
			/** @todo CPU_HAS_SNOOPING problem?!?! */
			
			/* Copy UDP, IP header to user's buffer*/
			memcpy(b, sc->rxdesc[sc->rx_ptr].addr, 42);
			
			/*Copy received data to user's buffer*/
			memcpy(d, &buffer[42], len-42);
		}
		
		/*reactivate descriptor*/
		if (sc->rx_ptr == sc->rxbufs - 1) {
		
			/* This is the last descriptor so enable it and activate WRAP*/
			sc->rxdesc[sc->rx_ptr].ctrl = GRETH_RXD_ENABLE | GRETH_RXD_WRAP;
		} else {
		
			/* just enable the descriptor */
			sc->rxdesc[sc->rx_ptr].ctrl = GRETH_RXD_ENABLE;
		}
		
		
		// printf("****************RE-ENABLING*****************\n");
		
		/*enable reception*/
		sc->regs->ctrl |= GRETH_CTRL_RXEN;
		
		/*increment descriptor*/
		sc->rx_ptr = (sc->rx_ptr + 1) % sc->rxbufs;
		
		/* if we read something*/
		if(len != 0){
			/* break cicle */
			exit = 1;
		}
	}
	
	return len;
}

/** 
 *  \param [in]  sc	: internal driver structure
 *  \param [in]  b : pointer to header
 *  \param [in]  c : header length in bytes
 *  \param [in]  data : pointer to data
 *  \param [in]  data_len: data length in bytes
 *
 *  \return number of bytes written
 *
 *  \brief writes data to ethernet
 **/
static int sendpacket (greth_softc_t *sc, char *b, int c, char *data, int data_len){
    
	unsigned int len;
	uint8_t *desc;
    
	/* Check if there are descriptor available */
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

    len = c + data_len;
	
    /* don't send long packets */
    if (c <= GRETH_MAXBUF_LEN) {
		
		/* Copy header*/
		memcpy(sc->txdesc[sc->tx_ptr].addr, b, c);
		
		/* cast to uint8_t so we can index using size */
		desc = (uint8_t *)sc->txdesc[sc->tx_ptr].addr;
		
		/* We can have a packet that only has header*/
		if(data_len > 0){
			
			/*copy data*/
			memcpy(&desc[c], data, data_len);
		}
		
		/* Enable descriptor*/
		if (sc->tx_ptr < sc->txbufs-1) {
			
			/* re enable descriptor and write total data length*/
			sc->txdesc[sc->tx_ptr].ctrl = GRETH_TXD_ENABLE | len;
		} else {
		
			/* this is last descriptor so also activate WRAP */
			sc->txdesc[sc->tx_ptr].ctrl = 
					GRETH_TXD_WRAP | GRETH_TXD_ENABLE | len;
		}
		
		/* Enable transmission*/
		sc->regs->ctrl = sc->regs->ctrl | GRETH_CTRL_TXEN;
		
		/* increment descriptor*/
		sc->tx_ptr = (sc->tx_ptr + 1) % sc->txbufs;
		
    }
	
	/* return number of bytes written */
	return c;
}

rtems_device_driver greth_initialize(rtems_device_major_number  major,
								     rtems_device_minor_number  minor,
								     void *arg)
{
	
	int device_found = 0;
	rtems_status_code status = RTEMS_SUCCESSFUL;
	greth_softc_t *sc;
	
	sc = &greth;
	memset(sc, 0, sizeof(greth_softc_t));
	
	
    /* Get user configuration */
    
    /** @todo Should we validade greth config or trust in the configuration */
    /* Store configuration parameters */
    sc->txbufs = TXRX_COUNT;
    sc->rxbufs = TXRX_COUNT;
    
    sc->regs = (void *)0xFF940000;
    
    /** @todo remove acceptBroadcast*/
    sc->acceptBroadcast = ACCEPT_BROADCAST;
    sc->tx_blocking = 0;
    sc->rx_blocking = 0;
    sc->wait_ticks = 1;
    
    /* Device was innited */
    sc->started = 1;
	
	
	return status;
	
}


rtems_device_driver greth_open(rtems_device_major_number  major,
							   rtems_device_minor_number  minor,
							   void *arg)
{
	rtems_status_code status = RTEMS_SUCCESSFUL;
	greth_softc_t *sc;
	sc = &greth;
	
	if(!sc->started)
		status = RTEMS_NOT_CONFIGURED;
	
	/*Initialize the eth core*/
	greth_initialize_hardware(sc);

	return status;
}


rtems_device_driver greth_close(rtems_device_major_number  major,
							   rtems_device_minor_number  minor,
							   void *arg)
{
	greth_softc_t *sc;
	sc = &greth;
	
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


rtems_device_driver greth_read(rtems_device_major_number  major,
							   rtems_device_minor_number  minor,
							   void *arg)
{
	greth_softc_t *sc;
	int count;
	
	
	/* User IO arguments*/
	greth_args_t *rw_args;
	
	sc = &greth;
	
	rw_args = (greth_args_t *) arg;
	
	if(!sc->started)
		return RTEMS_NOT_CONFIGURED;
	
	/* Verify user arguments consistency*/
	if ( rw_args->data == NULL){
		return RTEMS_INVALID_NAME;
	}
	
	/** @todo check for errors that cannot be attributed to a descriptor*/
	
	
	/* While we have read no data*/
	while ((count = greth_hw_receive(sc, rw_args->hdr, rw_args->data)) == 0) {
		
		/* Are we allowed to block?*/
		if (sc->rx_blocking) {
			
			/* wait a moment for any RX descriptors to get available */
			rtems_task_wake_after(sc->wait_ticks);
		} else {
			
			/* We can't block waiting, so we return */
			return RTEMS_RESOURCE_IN_USE;
		}
	}
	
	/*Write number of bytes that were written to the user's buffer*/
	rw_args->bytes_moved = count;
	
	return RTEMS_SUCCESSFUL;
}


rtems_device_driver greth_write(rtems_device_major_number  major,
							    rtems_device_minor_number  minor,
							    void *arg)
{	
	int count;
	greth_softc_t *sc;
	sc = &greth;
	
	
	/*User IO arguments: buffer and count*/
	greth_args_t *rw_args;
	rw_args = (greth_args_t *) arg;
	
	if(!sc->started)
		return RTEMS_NOT_CONFIGURED;
		
	if ((rw_args->hdr_len + rw_args->data_len) < 1){
		return RTEMS_INVALID_NAME;
	}
		
	/** @todo check for errors that cannot be attributed to a descriptor*/
	
	count = sendpacket(sc, rw_args->hdr, rw_args->hdr_len, rw_args->data,
					    rw_args->data_len);
	
	/*Write number of bytes that were written to the user's buffer*/
	rw_args->bytes_moved = count;
	
	return RTEMS_SUCCESSFUL;
}

rtems_device_driver greth_ioctl(rtems_device_major_number  major,
							    rtems_device_minor_number  minor,
							    void *arg)
{
	greth_softc_t *sc;
	sc = &greth;
	
	if(!sc->started)
		return RTEMS_NOT_CONFIGURED;
		
	
	return RTEMS_SUCCESSFUL;
}
