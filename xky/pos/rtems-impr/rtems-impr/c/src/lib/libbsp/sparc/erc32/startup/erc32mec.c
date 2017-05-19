/**
 *  @file
 *  erc32mec.c
 *
 *  @brief contains the ERC32 register map variable
 *
 *  Put this variable in a separate file so it is only linked in when needed.
 *
 *  COPYRIGHT (c) 2010.
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  8183    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SPARC_ERC32_BSP SPARC ERC32 BSP
 *  @{
 */

#include <rtems.h>
#include <bsp.h>

ERC32_Register_Map ERC32_MEC;

/**
 *  @}
 */
