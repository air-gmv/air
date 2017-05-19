/**
 *  @file
 *  amba.c
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
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5393    | 03/11/2009  | mcoutinho    | IPR 64
 *  5970    | 14/01/2010  | mcoutinho    | IPR 1053
 *  9619    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SPARC_LEON3_BSP SPARC LEON3 BSP
 *  @{
 */

#include <bsp.h>


amba_confarea_type amba_conf;

/**
 * @brief pointers to Interrupt Controller configuration registers
 */
volatile LEON3_IrqCtrl_Regs_Map *LEON3_IrqCtrl_Regs;


#if defined(RTEMS_MULTIPROCESSING)

int LEON3_Cpu_Index = 0;

/**
 *  @brief get register ASR 17 used for multiprocessing
 *
 *  BSP predriver hook.  Called just before drivers are initialized.
 *  Used to scan system bus. Probes for AHB masters, AHB slaves and 
 *  APB slaves. Addresses to configuration areas of the AHB masters,
 *  AHB slaves, APB slaves and APB master are storeds in 
 *  amba_ahb_masters, amba_ahb_slaves and amba.
 */
unsigned int getasr17();

asm(" .text  \n"
    "getasr17:   \n"
    "retl \n"
    "mov %asr17, %o0\n");

#endif


void bsp_leon3_predriver_hook(void)
{
    /* number of slaves found */
    int i;

#if defined(RTEMS_MULTIPROCESSING)

    /* register ASR 17 */
    unsigned int tmp;

#endif

    /* amba APB device for the IRQ MP */
    amba_apb_device dev;

    /* Scan the AMBA Plug&Play info at the default LEON3 area */
    amba_scan(&amba_conf , LEON3_IO_AREA , NULL);

    /* Find LEON3 Interrupt controller */
    i = amba_find_apbslv(&amba_conf , VENDOR_GAISLER , GAISLER_IRQMP , &dev);

    /* if found any interrupt controller */
    if(i > 0)
    {
        /* found APB IRQ_MP Interrupt Controller */

        /* get the start address */
        LEON3_IrqCtrl_Regs = ( volatile LEON3_IrqCtrl_Regs_Map * ) dev.start;

        /* if multiprocessing is enabled */
#if defined(RTEMS_MULTIPROCESSING)

        /* if use multiprocessing */
        if(Configuration.User_multiprocessing_table != NULL)
        {
            /* get ASR 17 */
            tmp = getasr17();

            /* determine the CPU index based on the ASR 17 */
            LEON3_Cpu_Index = ( tmp >> 28 ) & 3;
        }

#endif
        /* initialize the interrupt handlers */
        bsp_spurious_initialize();
    }

    /* find GP Timer */
    i = amba_find_apbslv(&amba_conf , VENDOR_GAISLER , GAISLER_GPTIMER , &dev);

    /* if found any GP Timer */
    if(i > 0)
    {
        /* get the start address for the timer register */
        LEON3_Timer_Regs = ( volatile LEON3_Timer_Regs_Map * ) dev.start;
    }
}

/**  
 *  @}
 */
