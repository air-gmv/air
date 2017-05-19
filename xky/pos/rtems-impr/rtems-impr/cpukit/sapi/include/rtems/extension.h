/**
 *  @file
 *  extension.h
 *
 *  @brief all the constants, structures, and prototypes associated with the 
 *  User Extension Manager.  This manager provides a mechanism for manipulating
 *  sets of user-defined extensions.
 *
 *  Directives provided are:
 *\n
 *     + create user extension set\n
 *     + get ID of user extension set\n
 *     + delete user extension set\n
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_API Super API
 *  @{
 */

/**
 *  @defgroup SUPER_API_EXTENSION Extension Manager
 *
 *  @brief The Extension Manager component consists on an API that allows the user to
 *  place handlers that are called at specific instants of the system lifetime
 *  (e.g. thread context switch, thread creation, fatal error, etc).
 *  @{
 */

#ifndef _RTEMS_EXTENSION_H
#define _RTEMS_EXTENSION_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/score/object.h>
#include <rtems/score/userext.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

   /*
    *  Extension related types
    */

   typedef User_extensions_routine rtems_extension;
   typedef User_extensions_thread_create_extension rtems_task_create_extension;
   typedef User_extensions_thread_delete_extension rtems_task_delete_extension;
   typedef User_extensions_thread_start_extension rtems_task_start_extension;
   typedef User_extensions_thread_restart_extension rtems_task_restart_extension;
   typedef User_extensions_thread_switch_extension rtems_task_switch_extension;
   typedef User_extensions_thread_post_switch_extension
   rtems_task_post_switch_extension;
   typedef User_extensions_thread_begin_extension rtems_task_begin_extension;
   typedef User_extensions_thread_exitted_extension rtems_task_exitted_extension;
   typedef User_extensions_fatal_extension rtems_fatal_extension;

   typedef User_extensions_Table rtems_extensions_table;

   /**
    *  @brief define the information control block used to manage
    *  this class of objects.
    */
   extern Objects_Information _Extension_Information;

   /**
    *  @brief control block used to manage each extension.
    */
   typedef struct
   {
      Objects_Control Object;
      User_extensions_Control Extension;
   } Extension_Control;

   /**
    *  @brief initialize the Extension Manager
    *
    *  This routine initializes all extension manager related data structures.
    *
    *  @param[in] maximum_extensions number of extensions to initialize
    */
   void _Extension_Manager_initialization(
                                          uint32_t maximum_extensions
                                          );

   /**
    *  @brief create an extension
    *
    *  This directive creates a extension and performs some initialization.
    *
    *  @param[in] name extension name
    *  @param[in] extension_table pointer to extension set information
    *  @param[out] id extension id
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_extension_create(
                                            rtems_name name ,
                                            rtems_extensions_table *extension_table ,
                                            Objects_Id *id
                                            );

   /**
    *  @brief ident an extension
    *
    *  This directive returns the system ID associated with
    *  the extension name.
    *
    *  @param[in] name user defined message queue name
    *  @param[out] id message queue id
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_extension_ident(
                                           rtems_name name ,
                                           Objects_Id *id
                                           );

   /**
    *  @brief delete an extension
    *
    *  This directive allows a thread to delete a extension.
    *
    *  @param[in] id extension id
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_extension_delete(
                                            Objects_Id id
                                            );

#ifndef __RTEMS_APPLICATION__
#include <rtems/extension.inl>
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

