/**
 *  @file
 *  amba.h
 *
 *  @brief AMBA Plag & Play Bus Driver Macros
 *
 *  Macros used for AMBA Plug & Play bus scanning
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
 *  8183    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SPARC_LEON3_BSP SPARC LEON3 BSP
 *  @{
 */

#ifndef __AMBA_H__
#define __AMBA_H__

#define LEON3_IO_AREA 0xfff00000
#define LEON3_CONF_AREA 0xff000
#define LEON3_AHB_SLAVE_CONF_AREA (1 << 11)

#define LEON3_AHB_CONF_WORDS 8
#define LEON3_APB_CONF_WORDS 2
#define LEON3_AHB_MASTERS 64
#define LEON3_AHB_SLAVES 64
#define LEON3_APB_SLAVES 16
#define LEON3_APBUARTS 8

#include <ambapp.h>

#ifdef __cplusplus
extern "C"
{
#endif

   /**
    * @brief structure containing address to devices found on the Amba Plug&Play bus
    */
   extern amba_confarea_type amba_conf;

#ifdef __cplusplus
}
#endif

#endif /* __AMBA_H__ */

/**
 *  @}
 */
