/**
 *  @file
 *  attr.h
 *
 *  @brief all information about the Object Attributes Handler.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  7369    | 14/04/2010  | mcoutinho    | IPR 1886
 *  8322    | 21/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_COMMON Common
 *  @{
 */

#ifndef _RTEMS_RTEMS_ATTR_H
#define _RTEMS_RTEMS_ATTR_H

#ifdef __cplusplus
extern "C"
{
#endif


   /* types */


   /**
    * @brief RTEMS attribute
    */
   typedef uint32_t rtems_attribute;


   /* constants */


   /**
    * @brief default attributes
    */
#define RTEMS_DEFAULT_ATTRIBUTES  0x00000000

   /**
    * @brief local resource
    */
#define RTEMS_LOCAL               0x00000000 


#ifdef RTEMS_MULTIPROCESSING

   /**
    * @brief global resource
    */
#define RTEMS_GLOBAL              0x00000002 

#endif


   /**
    * @brief queueing discipline is FIFO
    */
#define RTEMS_FIFO                0x00000000

   /**
    * @brief queueing discipline is by priority
    */
#define RTEMS_PRIORITY            0x00000004 


   /* RTEMS Task Specific Attributes */


   /**
    * @brief attribute stating no FP is necessary
    */
#define RTEMS_NO_FLOATING_POINT   0x00000000

   /**
    * @brief attribute stating FP is necessary
    */
#define RTEMS_FLOATING_POINT      0x00000001 


   /* RTEMS Semaphore Specific Attributes */

   /**
    * @brief semaphore class
    */
#define RTEMS_SEMAPHORE_CLASS         0x00000030

   /**
    * @brief counting semaphore
    */
#define RTEMS_COUNTING_SEMAPHORE      0x00000000

   /**
    * @brief binary semaphore
    */
#define RTEMS_BINARY_SEMAPHORE        0x00000010

   /**
    * @brief simple binary semaphore
    */
#define RTEMS_SIMPLE_BINARY_SEMAPHORE 0x00000020

   /**
    * @brief dont use priority inheritance (normal inheritance nor ceiling)
    * protocols
    */
#define RTEMS_NO_INHERIT_PRIORITY     0x00000000

   /**
    * @brief use inherit priority protocol
    */
#define RTEMS_INHERIT_PRIORITY        0x00000040

   /**
    * @brief ceiling protocol is not used
    */
#define RTEMS_NO_PRIORITY_CEILING     0x00000000

   /**
    * @brief use ceiling priority protocol
    */
#define RTEMS_PRIORITY_CEILING        0x00000080


   /* RTEMS Internal Task Specific Attributes */
#define RTEMS_APPLICATION_TASK        0x00000000

   /**
    * @brief attribute that specifies an RTEMS internal task
    **/
#define RTEMS_SYSTEM_TASK             0x00008000


   /* determine the value of attributes when atributes are not supported */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )

   /**
    * @brief all attributes are supported (FP is supported)
    */
#define ATTRIBUTES_NOT_SUPPORTED       0

#else

   /**
    * @brief FP attribute not supported
    */
#define ATTRIBUTES_NOT_SUPPORTED       RTEMS_FLOATING_POINT

#endif

   /* determine the default attributes required */
#if ( CPU_ALL_TASKS_ARE_FP == TRUE )

   /**
    * @brief FP is required for all tasks
    */
#define ATTRIBUTES_REQUIRED            RTEMS_FLOATING_POINT

#else

   /**
    * @brief no attribute is required for all tasks
    */
#define ATTRIBUTES_REQUIRED            0

#endif

   /**
    *  @brief initialize the attributes
    *
    *  This routine performs initialization for this handler.
    *
    *  NOTE: There is no initialization required in C.  Conditional compilation
    *        takes care of this in C.
    */
#define _Attributes_Handler_initialization()

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/attr.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

/**
 *  @}
 */

/**
 *  @}
 */
