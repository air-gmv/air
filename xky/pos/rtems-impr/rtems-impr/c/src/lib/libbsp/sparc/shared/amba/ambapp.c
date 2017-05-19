/**
 *  @file
 *  ambapp.c
 *
 *  @brief AMBA Plag & Play Bus Driver
 *
 *  This driver hook performs bus scanning.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  234     | 09/10/2008  | mcoutinho    | IPR 66
 *  2470    | 15/04/2009  | mcoutinho    | IPR 452
 *  3972    | 23/07/2009  | mcoutinho    | IPR 608
 *  4342    | 15/09/2009  | mcoutinho    | IPR 610
 *  5248    | 30/10/2009  | mcoutinho    | IPR 828
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  7671    | 06/05/2010  | mcoutinho    | IPR 2277
 *  9619    | 23/02/2011  | mcoutinho    | IPR 451
 *  9872    | 16/03/2011  | aconstantino | SPR 2819
 *  $Rev: 9877 $ | $Date: 2011-03-18 18:39:36 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2846
 *
 **/

/**
 *  @addtogroup SHARED_BETWEEN_SPARC_BSPS Shared between SPARC BSPs
 *  @{
 */

#include <bsp.h>
#include <ambapp.h>

/**
 *  @brief insert an amba device
 **/
#define amba_insert_device(tab, address) \
{ \
  if (*(address)) \
  { \
    (tab)->addr[(tab)->devnr] = (address); \
    (tab)->devnr ++; \
  } \
} while(0)

/**
 * @brief insert an amba apb device
 **/
#define amba_insert_apb_device(tab, address, apbmst) \
{ \
  if (*(address)) \
  { \
    (tab)->addr[(tab)->devnr] = (address); \
    (tab)->apbmst[(tab)->devnr] = (apbmst); \
    (tab)->devnr ++; \
  } \
} while(0)


unsigned int amba_addr_from(struct amba_mmap *mmaps , unsigned int address)
{
    /* check if the maps are valid */
    if(!mmaps)
    {
        /* return the address itself */
        return address;
    }

    /* iterate through every map (finish when the last map has 0 size) */
    while(mmaps->size)
    {
        /* if the address is valid */
        if(( address >= mmaps->remote_amba_adr )
           && ( address <= ( mmaps->remote_amba_adr + ( mmaps->size - 1 ) ) ))
        {
            /* return the address translated */
            return (address - mmaps->remote_amba_adr ) +mmaps->cpu_adr;
        }

        /* go to the next mmaps position */
        mmaps++;
    }

    /* return success */
    return 1;
}

void amba_scan_bridge(amba_confarea_type * amba_conf, unsigned int ioarea){
			   
    /* address to configuration area */
    unsigned int *cfg_area;

    /* AHB slave iterator */
    int i;

    /* number of maximum AHB devices to search for */
    const int maxloops = 64;
	
    /* plug and play area is ussually the last 4 Kb */
    cfg_area = (unsigned int *) ( ioarea | AMBA_CONF_AREA );

	/* loop for the number of devices */
    for(i = 0; ((i < maxloops) && (amba_conf->ahbmst.devnr < AMBA_AHB_MASTERS)); i++){
	
        /* insert the device in the config area */
        amba_insert_device(&amba_conf->ahbmst , cfg_area);

        /* increment the config area */
        cfg_area += AMBA_AHB_CONF_WORDS;
    }

	/* set the config area to the io area (slave) */
    cfg_area = (unsigned int *) ( ioarea | AMBA_CONF_AREA 
								| AMBA_AHB_SLAVE_CONF_AREA );

    /* loop for the number of devices */
    for(i = 0; ((i < maxloops) && (amba_conf->ahbslv.devnr < AMBA_AHB_SLAVES)); i++){
	
        /* insert the device in the config area */
        amba_insert_device(&amba_conf->ahbslv, cfg_area);

        /* increment the config area */
        cfg_area += AMBA_AHB_CONF_WORDS;
    }
	
}

void amba_scan(amba_confarea_type * amba_conf , unsigned int ioarea ,
               struct amba_mmap *mmaps)
{
    /* address to configuration area */
    unsigned int *cfg_area;

    /* AHB slave mem bar value */
    unsigned int mbar;

    /* AHB slave configuration word */
    unsigned int conf;

    /* AHB slave iterator */
    int i;
    
    /* APB slave device iterator */
    int j;

    /* APB master address */
    unsigned int apbmst;

    /* number of maximum AHB devices to search for */
    int maxloops;
	
	/* bridge address */
	unsigned int bridge_addr;


    /* if root is set */
    if(amba_conf->notroot == 0)
    {
        /* scan first bus for 64 devices*/
        maxloops = 64;
    }
    else
    {
        /* else, scan for the rest for 16 devices */
        maxloops = 16;
    }

    /* initialize the AHB and APB number of devices found to zero */
    amba_conf->ahbmst.devnr = 0;
    amba_conf->ahbslv.devnr = 0;
    amba_conf->apbslv.devnr = 0;

    /* initialize the config area (master) */
    cfg_area = (unsigned int *) ( ioarea | AMBA_CONF_AREA );

    /* initialize the io area */
    amba_conf->ioarea = ioarea;

    /* initialize the mmaps */
    amba_conf->mmaps = mmaps;

    /* loop for the number of devices */
    for(i = 0; i < maxloops; i++)
    {
        /* insert the device in the config area */
        amba_insert_device(&amba_conf->ahbmst , cfg_area);

        /* increment the config area */
        cfg_area += AMBA_AHB_CONF_WORDS;
    }

    /* set the config area to the io area (slave) */
    cfg_area = (unsigned int *) ( ioarea | AMBA_CONF_AREA | AMBA_AHB_SLAVE_CONF_AREA );

    /* loop for the number of devices */
    for(i = 0; i < maxloops; i++)
    {
        /* insert the device in the config area */
        amba_insert_device(&amba_conf->ahbslv , cfg_area);

        /* incremene the config area */
        cfg_area += AMBA_AHB_CONF_WORDS;
    }

    /* loop for the number of slave devices */
    for(i = 0; i < amba_conf->ahbslv.devnr; i++)
    {
        /* get the configuration for the device */
        conf = amba_get_confword(amba_conf->ahbslv , i , 0);

        mbar = amba_ahb_get_membar(amba_conf->ahbslv , i , 0);
		
		/* 
		 * Found AHB->AHB bus bridge, scan it if more free amba_confarea_type
		 * are available 
		 */
		if ((amba_vendor(conf) == VENDOR_GAISLER) && 
		    (amba_device(conf) == GAISLER_AHB2AHB)){
		
			/* custom 1 contains the IOAREA for the bridge */
			bridge_addr = amba_ahb_get_custom(amba_conf->ahbslv, i, 1);
			
			/* If the bridge is present search it! */
			if ((amba_ver(conf) != 0) && (bridge_addr != 0)){
				
				/* scan AMBA bus over AHB -> AHB bridge */
				amba_scan_bridge(amba_conf, bridge_addr);
			}
		
		/* found an AHB->APB bus, lets find the APB devices */		
		} else if(( amba_vendor(conf) == VENDOR_GAISLER ) && 
				  ( amba_device(conf) == GAISLER_APBMST )){
            
			apbmst = amba_membar_start(mbar);

            /* if the result of calling amba_addr_from is 1, no available
             * memory translation available, it will not be able to access
             * Plug&Play information for this AHB/APB bridge so skip it.*/
            if(( apbmst = amba_addr_from(mmaps, apbmst) ) == 1)
            {
                /* skip it */
                continue;
            }

            /* initialize the config area to APB Master */
            cfg_area = (unsigned int *) ( apbmst | AMBA_CONF_AREA );

            /* iterate for the APB slave devices */
            for(j = 0; ( amba_conf->apbslv.devnr < AMBA_APB_SLAVES ) && ( j < AMBA_APB_SLAVES ); j++)
            {
                /* insert the APB device */
                amba_insert_apb_device(&amba_conf->apbslv , cfg_area , apbmst);

                /* increment the config area */
                cfg_area += AMBA_APB_CONF_WORDS;
            }
        }
    }
}


/**** APB Slaves ****/

int amba_find_apbslv(amba_confarea_type * amba_conf , int vendor , int device ,
                     amba_apb_device * dev)
{
    /* APB device configuration word */
    unsigned int conf;

    /* APB slave mem bar */
    unsigned int iobar;

    /* iterator index through the APB slave devices */
    int i;


    /* iterate through the APB slave devices */
    for(i = 0; i < amba_conf->apbslv.devnr; i++)
    {
        /* get the configuration area */
        conf = amba_get_confword(amba_conf->apbslv , i , 0);

        /* check the device vendor */
        if(( amba_vendor(conf) == vendor ) && ( amba_device(conf) == device ))
        {
            /* get the io mem bar */
            iobar = amba_apb_get_membar(amba_conf->apbslv , i);

            /* get the device start address */
            dev->start = amba_iobar_start(amba_conf->apbslv.apbmst[i] , iobar);

            /* get the devide interrupt number */
            dev->irq = amba_irq(conf);

            /* found the device */
            return 1;
        }
    }

    /* did not find the device */
    return 0;
}


/**  
 *  @}
 */
